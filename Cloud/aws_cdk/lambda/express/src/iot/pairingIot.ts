import * as iot from "@aws-sdk/client-iot-data-plane";
import { Utils } from "../utils/utils";
import { AppIotError } from "../utils/appErrors";

const region = Utils.getEnv("AWS_APP_REGION");
const accountId = Utils.getEnv("AWS_APP_ACCOUNT_ID");
const stackAndEnv = Utils.getEnv("STACK_AND_ENV");

const client = new iot.IoTDataPlaneClient({
  region: region,
});

export async function publishUser(devId: string, email: string) {
  const topic = `${stackAndEnv}/device/${devId}/pairing/user`;

  const mqttMsg = {
    email,
  };

  const command: iot.PublishCommandInput = {
    topic,
    payload: Buffer.from(JSON.stringify(mqttMsg)),
    qos: 0,
  };

  try {
    const data: iot.PublishCommandOutput = await client.send(
      new iot.PublishCommand(command)
    );
    console.log(
      `Iot publish data dev ${devId} paired with user ${email} \n`,
      data
    );
  } catch (err: any) {
    console.log("IOT publish error !!");

    console.log(err);

    throw new AppIotError("Failed to publish message to device", err);
  }
}
