import { Args } from "./cmdTypes";
import * as iot from "@aws-sdk/client-iot";
import { AWS_REGION } from "./cmdCommonParams";

const WOOTCH_DEVICE_TYPE = "wootch_device";
const client = new iot.IoTClient({ region: AWS_REGION });

function nameFromId(id: string) {
  return `wootch_dev_${id.toUpperCase()}`;
}

export async function createDeviceCmd(argv: Args) {
  console.log(`creating new device ${argv.id}`);
  const params: iot.CreateThingCommandInput = {
    thingName: nameFromId(argv.id),
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingCommand(params);
  let data: iot.CreateThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    if (error.message.includes("Type")) {
      console.log("ERROR : Please create a thing type");
    } else {
      console.log(JSON.stringify(error, null, 2));
    }
    return;
  }
  console.log(`device ${argv.id} successfullly created`);
}

export async function createDeviceTypeCmd() {
  console.log(`creating new device type: "${WOOTCH_DEVICE_TYPE}"`);
  const params: iot.CreateThingTypeCommandInput = {
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingTypeCommand(params);
  let data: iot.CreateThingTypeCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    console.log(error);
    return;
  }
  console.log(`device type ${WOOTCH_DEVICE_TYPE} successfullly created`);
}

export async function deleteDeviceCmd(argv: Args) {
  console.log(`deleting device ${argv.id}`);
  if (!(await deviceExist(argv.id))) {
    return;
  }
  const params: iot.DeleteThingCommandInput = {
    thingName: nameFromId(argv.id),
  };
  const command = new iot.DeleteThingCommand(params);
  let data: iot.DeleteThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    if (error.message.includes("Type")) {
      console.log("ERROR : Please create a thing type");
    } else {
      console.log(JSON.stringify(error, null, 2));
    }
    return;
  }
  console.log(`device ${argv.id} successfullly deleted`);
}

export async function searchDeviceCmd(argv: Args) {
  try {
    const data = await describeDevice(argv.id);
    console.log(JSON.stringify(data, null, 2));
  } catch (error) {
    return;
  }
}

async function describeDevice(id: string) {
  console.log(`searching for device ${nameFromId(id)}`);
  const params: iot.DescribeThingCommandInput = {
    thingName: nameFromId(id),
  };
  const command = new iot.DescribeThingCommand(params);
  let data: iot.DescribeThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    if (error["$metadata"].httpStatusCode != 404) {
      console.log("Unexpected error on DescribeThing");
      console.log(error);
    }
    console.log(`device ${nameFromId(id)} does not exist`);
    throw error;
  }
  return data;
}

async function deviceExist(id: string): Promise<boolean> {
  try {
    await describeDevice(id);
  } catch (error) {
    return false;
  }
  return true;
}
