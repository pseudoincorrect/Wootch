import { mqtt, auth, http, io, iot } from "aws-iot-device-sdk-v2";
import { TextDecoder } from "util";
import * as secrets from "./secrets";

// npx ts-node sendSomeData -e xxx-ats.iot.eu-west-1.amazonaws.com -r certificates/aws-root-ca.pem -c certificates/certificate.pem.crt -k  certificates/private.pem.key -C xxx -W true

// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts


const messageCount: number = 10;
const testTopic: string = "topic/test";
const testMessage: string = "Hellow World !";
// choices: "fatal", "error", "warn", "info", "debug", "trace", "none"
const verbosity: string = "info";
const use_websocket: boolean = true;
const signing_region: string = "eu-west-1";


async function execute_session(
  connection: mqtt.MqttClientConnection
) {
  return new Promise(async (resolve, reject) => {
    try {
      const decoder = new TextDecoder("utf8");
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
        const message = JSON.parse(json);
        if (message.sequence == messageCount) {
          resolve(null);
        }
      };

      await connection.subscribe(testTopic, mqtt.QoS.AtLeastOnce, on_publish);

      for (let op_idx = 0; op_idx < messageCount; ++op_idx) {
        const publish = async () => {
          const msg = {
            message: testMessage,
            sequence: op_idx + 1,
          };
          const json = JSON.stringify(msg);
          connection.publish(testTopic, json, mqtt.QoS.AtLeastOnce);
        };
        setTimeout(publish, op_idx * 1000);
      }
    } catch (error) {
      reject(error);
    }
  });
}

async function main() {
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
      region: signing_region,
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

  if (secrets.ca_file != null) {
    config_builder.with_certificate_authority_from_path(
      undefined,
      secrets.ca_file
    );
  }

  config_builder.with_clean_session(false);
  config_builder.with_client_id(
    secrets.client_id || "test-" + Math.floor(Math.random() * 100000000)
  );
  config_builder.with_endpoint(secrets.endpoint);

  // force node to wait 60 seconds before killing itself, promises do not keep node alive
  const timer = setTimeout(() => {}, 60 * 1000);

  const config = config_builder.build();
  const client = new mqtt.MqttClient(client_bootstrap);
  const connection = client.new_connection(config);

  await connection.connect();
  await execute_session(connection);
  await connection.disconnect();

  // Allow node to die if the promise above resolved
  clearTimeout(timer);
}


// Run the example
main();