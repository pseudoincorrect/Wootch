import { auth, io, iot, mqtt } from "aws-iot-device-sdk-v2";

import * as secrets from "./secrets/secrets";

// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts

const verbosity: string = "info";

/**
 * Setup and connect to MQTT broker (AWS) and start the app process
 */
export async function executeSession(
  session: (con: mqtt.MqttClientConnection) => void
) {
  if (verbosity != "none") {
    const level: io.LogLevel = parseInt(
      io.LogLevel[verbosity.toUpperCase() as any]
    );
    io.enable_logging(level);
  }

  const client_bootstrap = new io.ClientBootstrap();

  let config_builder = null;
  config_builder = iot.AwsIotMqttConnectionConfigBuilder.new_with_websockets({
    region: secrets.AWS_REGION,
    credentials_provider:
      auth.AwsCredentialsProvider.newDefault(client_bootstrap),
  });

  config_builder.with_certificate_authority_from_path(
    undefined,
    "./certificatesAndSecrets/aws-root-ca.pem"
  );

  config_builder.with_clean_session(false);
  config_builder.with_client_id(
    secrets.AWS_THING_NAME_SPOOF ||
      "test-" + Math.floor(Math.random() * 100000000)
  );
  config_builder.with_endpoint(secrets.AWS_IOT_ENDPOINT);

  const config = config_builder.build();
  const client = new mqtt.MqttClient(client_bootstrap);
  const connection = client.new_connection(config);

  // force node to wait 60 seconds before killing itself, promises do not keep node alive
  const timer = setTimeout(() => {}, 10 * 1000);

  await connection.connect();
  console.log("Connected");

  await session(connection);
  console.log("Data sent");

  await connection.disconnect();
  console.log("Disconnected");

  // Allow node to die if the promise above resolved
  clearTimeout(timer);
}
