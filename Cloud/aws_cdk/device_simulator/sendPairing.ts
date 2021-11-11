import { mqtt } from "aws-iot-device-sdk-v2";
import { TextDecoder } from "util";
import { v4 as uuidv4 } from "uuid";

import * as secrets from "./certificatesAndSecrets/secrets";
import { executeSession } from "./communication";

// based on example : https://github.com/aws/aws-iot-device-sdk-js-v2/blob/main/samples/node/pub_sub/index.ts

const notifTopic: string = `WootchDev/device/${secrets.spoof_wootch_client_id}/pairing/request`;

interface PairingMessage {
  secret: string;
}

export async function sendPairing(connection: mqtt.MqttClientConnection) {
  let uuidSecrets: string;
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

      uuidSecrets = uuidv4().replace(/-/g, "").toUpperCase().slice(0, 6);

      const Pairing: PairingMessage = { secret: uuidSecrets };
      const json = JSON.stringify(Pairing);

      await connection.publish(notifTopic, json, mqtt.QoS.AtLeastOnce);

      resolve(null);
    } catch (error) {
      reject(error);
    }

    console.log("*********************************");
    console.log("*********************************");
    console.log(`*  Pairing Secret: ${uuidSecrets}`);
    console.log("*********************************");
    console.log("*********************************");
  });
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  console.log("Wootch Simulator will send a notification of activity !");
  executeSession(sendPairing);
}
