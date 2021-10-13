import { Interface } from "readline";
import * as iotEvent from "./iotEvent";

const secuLvlPattern = RegExp(
  /^WootchDev\/device\/[a-zA-Z0-9]+\/data\/securityLvl/
);
const maxAccPattern = RegExp(/^WootchDev\/device\/[a-zA-Z0-9]+\/data\/maxAcc/);

interface IotEvent {
  sequence: number;
  message: string;
  topic: string;
  clientid: string;
}

async function iotReceive(event: IotEvent, context: any) {
  console.log("IoT event packet");
  console.log(JSON.stringify(event, null, 2));
}

export const handler = iotReceive;
