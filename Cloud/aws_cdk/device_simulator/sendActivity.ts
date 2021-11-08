import { mqtt } from "aws-iot-device-sdk-v2";
import { TextDecoder } from "util";

import * as secrets from "./certificatesAndSecrets/secrets";
import { executeSession } from "./communication";

// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts

// const notifTopic: string = `WootchDev/device/${secrets.client_id}/data`;
const notifTopic: string = `WootchDev/device/${secrets.spoof_wootch_client_id}/data/activity`;

enum WATCH_LEVEL {
  none = "NONE",
  low = "LOW",
  high = "HIGH",
}

interface ActivityMessage {
  watchLvl: string;
  maxAcc: number;
  accThresh: number;
}

const notifMessage: ActivityMessage = {
  watchLvl: WATCH_LEVEL.low,
  maxAcc: 0.7,
  accThresh: 0.3,
};

/**
 * Send a notification to AWS
 * @param connection
 * @returns
 */
export async function sendActivity(connection: mqtt.MqttClientConnection) {
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

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  console.log("Wootch Simulator will send an activity message !");
  executeSession(sendActivity);
}
