import { mqtt } from "aws-iot-device-sdk-v2";
import { TextDecoder } from "util";

import * as secrets from "./certificatesAndSecrets/secrets";
import { executeSession } from "./communication";

// npx ts-node sendSomeData -e xxx-ats.iot.eu-west-1.amazonaws.com -r certificates/aws-root-ca.pem -c certificates/certificate.pem.crt -k  certificates/private.pem.key -C xxx -W true
// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts

const messageCount: number = 3;
const testTopic: string = `WootchDev/device/${secrets.client_id}/data`;
const testMessage: string = "I am Wootching !";

export async function sendSomeData(connection: mqtt.MqttClientConnection) {
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

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  console.log("Wootch Simulator will send a notification of activity !");
  executeSession(sendSomeData);
}
