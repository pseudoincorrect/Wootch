import { mqtt, auth, io, iot } from "aws-iot-device-sdk-v2";
import { TextDecoder } from "util";
import * as secrets from "./certificatesAndSecrets/secrets";

// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts

// const notifTopic: string = `WootchDev/device/${secrets.client_id}/data`;
const notifTopic: string = `WootchDev/device/${secrets.spoof_wootch_client_id}/data/activity`;

enum WATCH_LEVEL {
  none = "NONE",
  low = "LOW",
  high = "HIGH",
}

const notifMessage: object = {
  watchLvl: WATCH_LEVEL.low,
  maxAcc: 0.7,
  threshold: 0.3,
};

// choices: "fatal", "error", "warn", "info", "debug", "trace", "none"
const verbosity: string = "info";
const use_websocket: boolean = true;

/**
 * Send a notification to AWS
 * @param connection
 * @returns
 */
async function execute_session(connection: mqtt.MqttClientConnection) {
  return new Promise(async (resolve, reject) => {
    try {
      const decoder = new TextDecoder("utf8");

      // Function to handle MQTT publish (reception)
      const on_publish = async (
        topic: string,
        payload: ArrayBuffer,
        dup: boolean,
        qos: mqtt.QoS,
        retain: boolean
      ) => {
        const json = decoder.decode(payload);
        console.log(
          `Publish received. topic:"${topic}" dup:${dup} qos:${qos} retain:${retain}`
        );
        console.log(json);
      };
      // subscribe to the same topic we will send data
      await connection.subscribe(notifTopic, mqtt.QoS.AtLeastOnce, on_publish);

      const json = JSON.stringify(notifMessage);
      await connection.publish(notifTopic, json, mqtt.QoS.AtLeastOnce);
      resolve(null);
    } catch (error) {
      reject(error);
    }
  });
}

/**
 * Setup and connect to MQTT broker (AWS) and start the app process
 */
async function sendIt() {
  if (verbosity != "none") {
    const level: io.LogLevel = parseInt(
      io.LogLevel[verbosity.toUpperCase() as any]
    );
    io.enable_logging(level);
  }

  const client_bootstrap = new io.ClientBootstrap();

  let config_builder = null;
  if (use_websocket) {
    config_builder = iot.AwsIotMqttConnectionConfigBuilder.new_with_websockets({
      region: secrets.aws_region,
      credentials_provider:
        auth.AwsCredentialsProvider.newDefault(client_bootstrap),
    });
  } else {
    config_builder =
      iot.AwsIotMqttConnectionConfigBuilder.new_mtls_builder_from_path(
        secrets.cert,
        secrets.key
      );
  }

  config_builder.with_certificate_authority_from_path(
    undefined,
    "./certificatesAndSecrets/aws-root-ca.pem"
  );

  config_builder.with_clean_session(false);
  config_builder.with_client_id(
    secrets.spoof_wootch_client_id ||
      "test-" + Math.floor(Math.random() * 100000000)
  );
  config_builder.with_endpoint(secrets.endpoint);

  // force node to wait 60 seconds before killing itself, promises do not keep node alive
  const timer = setTimeout(() => {}, 10 * 1000);

  const config = config_builder.build();
  const client = new mqtt.MqttClient(client_bootstrap);
  const connection = client.new_connection(config);

  await connection.connect();
  console.log("Connected");

  await execute_session(connection);
  console.log("Data sent");

  await connection.disconnect();
  console.log("Disconnected");

  // Allow node to die if the promise above resolved
  clearTimeout(timer);
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  console.log("Wootch Simulator will send a notification of activity !");
  sendIt();
}
