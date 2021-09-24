import * as iotEvent from "./iotEvent";

async function iotReceive(event: any, context: any) {
  console.log("IoT event packet");
  console.log(JSON.stringify(event, null, 2));
}

export const handler = iotReceive;
