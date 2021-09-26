import { Args } from "./cmdTypes";
import * as iot from "@aws-sdk/client-iot";
import { AWS_REGION } from "./cmdCommonParams";

const WOOTCH_DEVICE_TYPE = "WOOTCH_DEVICE";
const client = new iot.IoTClient({ region: AWS_REGION });

interface CreateCertificateOutput {
  certificateArn: string;
  certificateId: string;
  certificatePem: string;
  keyPair: {
    PrivateKey: string;
    PublicKey: string;
  };
}

/**
 * Create a thing device on AWS IOT
 * @param argv cli parameters
 * @returns
 */
export async function createDeviceCmd(argv: Args) {
  try {
    await createDevice(argv.id);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Create a generic device type on AWS IOT
 */
export async function createDeviceTypeCmd() {
  try {
    await createDeviceType();
  } catch (error) {
    console.log(error);
  }
}

/**
 * Delete a thing on AWS IOT if it exists
 * @param argv cli parameter
 */
export async function deleteDeviceCmd(argv: Args) {
  try {
    await deleteDevice(argv.id);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Search a thing device and print its info
 * @param argv cli parameters
 */
export async function searchDeviceCmd(argv: Args) {
  try {
    const data = await describeDevice(argv.id);
    console.log(JSON.stringify(data, null, 2));
  } catch (error) {
    console.log(error);
  }
}

/**
 * Concatene a predefined string with and id
 * @param id Id of the device
 * @returns
 */
function nameFromId(id: string): string {
  return `wootch_dev_${id.toUpperCase()}`;
}

/**
 * Create a thing device on AWS IOT
 * @param id device ID
 * @returns
 */
async function createDevice(id: string) {
  console.log(`Creating new device ${id}`);
  const params: iot.CreateThingCommandInput = {
    thingName: nameFromId(id),
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingCommand(params);
  let data: iot.CreateThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    if (error.message.includes("TYPE")) {
      console.log("ERROR : Please create a thing type");
    } else {
      console.log(JSON.stringify(error, null, 2));
    }
    return;
  }
  console.log(`Success: device ${id} created`);
}

/**
 * Create a generic device type on AWS IOT
 */
async function createDeviceType() {
  console.log(`Creating new device type: '${WOOTCH_DEVICE_TYPE}'`);
  const params: iot.CreateThingTypeCommandInput = {
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingTypeCommand(params);
  let data: iot.CreateThingTypeCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    return;
  }
  console.log(`Success: device type ${WOOTCH_DEVICE_TYPE} created`);
}

/**
 * Delete a thing on AWS IOT if it exists
 * @param id device id
 */
async function deleteDevice(id: string) {
  console.log(`Deleting device ${id}`);
  if (!(await deviceExist(id))) {
    console.log(`Device ${nameFromId(id)} does not exist`);
    return;
  }
  const params: iot.DeleteThingCommandInput = {
    thingName: nameFromId(id),
  };
  const command = new iot.DeleteThingCommand(params);
  let data: iot.DeleteThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    return;
  }
  console.log(`Success: device ${id} deleted`);
}

/**
 * Search for a thing device on AWS IOT
 * @param id device id
 * @returns AWS IOT Device Data
 */
async function describeDevice(id: string): Promise<any> {
  console.log(`Searching for device ${nameFromId(id)}`);
  const params: iot.DescribeThingCommandInput = {
    thingName: nameFromId(id),
  };
  const command = new iot.DescribeThingCommand(params);
  let data: iot.DescribeThingCommandOutput;
  try {
    data = await client.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Success: device ${nameFromId(id)} found`);
  return data;
}

/**
 * Return whether a AWS IOT device exist
 * @param id device ID
 * @returns if the device exists
 */
async function deviceExist(id: string): Promise<boolean> {
  try {
    await describeDevice(id);
  } catch (error) {
    return false;
  }
  return true;
}

/**
 * Create a certificate to be attached to a device
 * @returns Certificate Info and associated Keys
 */
async function createCertificate(): Promise<CreateCertificateOutput> {
  console.log("Creating a certificate");
  const params: iot.CreateKeysAndCertificateCommandInput = {
    setAsActive: true,
  };
  const command = new iot.CreateKeysAndCertificateCommand(params);
  let data: iot.CreateKeysAndCertificateCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    throw error;
  }
  console.log("Success: certificate created");
  return {
    certificateArn: data.certificateArn,
    certificateId: data.certificateId,
    certificatePem: data.certificatePem,
    keyPair: {
      PrivateKey: data.keyPair!.PrivateKey,
      PublicKey: data.keyPair!.PublicKey,
    },
  } as CreateCertificateOutput;
}

/**
 * Delete a certificate from AWS IOT
 * @param id certificate ID
 * @returns 
 */
async function deleteCertificate(id: string): Promise<any> {
  console.log(`Deleting certificate ${id.slice(0, 5)}...`);
  const params: iot.DeleteCertificateCommandInput = {
    certificateId: id,
  };
  const command = new iot.DeleteCertificateCommand(params);
  let data: iot.DeleteCertificateCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Success: certificate ${id.slice(0, 5)}... deleted`);
  return data;
}

/**
 * Search for a AWS IOT certificate
 * @param id certificate ID
 * @returns certificate info
 */
async function describeCertificate(id: string): Promise<any> {
  console.log(`Searching certificate ${id.slice(0, 5)}...`);
  const params: iot.DescribeCertificateCommandInput = {
    certificateId: id,
  };
  const command = new iot.DescribeCertificateCommand(params);
  let data: iot.DescribeCertificateCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Success: certificate ${id.slice(0, 5)}... found`);
  return data;
}

/**
 * Deactivate a certificate, usually before deleting it
 * @param id certificate ID
 * @returns deletion data
 */
async function deactivateCertificate(id: string): Promise<any> {
  console.log(`Deactivating certificate ${id.slice(0, 5)}...`);
  const params: iot.UpdateCertificateCommandInput = {
    certificateId: id,
    newStatus: "INACTIVE",
  };
  const command = new iot.UpdateCertificateCommand(params);
  let data: iot.UpdateCertificateCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Success: certificate ${id.slice(0, 5)}... deactivated`);
  return data;
}

/**
 * Check if a certificate exists
 * @param id certificated
 * @returns whether or not a certificate exists
 */
async function certificateExist(id: string): Promise<boolean> {
  try {
    await describeCertificate(id);
  } catch (error) {
    return false;
  }
  return true;
}

/**
 * Attach a certificate to a device (AWS IOT)
 * @param devId device ID
 * @param certifId certificate ID
 * @returns completion info
 */
async function attachCertificateToDevice(
  devId: string,
  certifId: string
): Promise<any> {
  console.log(
    `attaching certificate ${certifId.slice(0, 5)}... to device ${devId}`
  );
  const params: iot.AttachThingPrincipalCommandInput = {
    thingName: nameFromId(devId),
    principal: "",
  };
  const command = new iot.AttachThingPrincipalCommand(params);
  let data: iot.AttachThingPrincipalCommandOutput;
  try {
    data = await client.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Succes: certificate ${certifId.slice(0, 5)}... attached to device ${devId}`
  );
  return data;
}

/**
 * Archaic end-to-end testing
 */
export async function testIt() {
  try {
    const cert = await createCertificate();
    console.log(JSON.stringify(cert, null, 2));
    let exist = await certificateExist(cert.certificateId);
    console.log(
      `certificate ${cert.certificateId.slice(0, 5)}... exists ? ${exist}`
    );
    await deactivateCertificate(cert.certificateId);
    await deleteCertificate(cert.certificateId);
    exist = await certificateExist(cert.certificateId);
    console.log(
      `certificate ${cert.certificateId.slice(0, 5)}... exists ? ${exist}`
    );
  } catch (error) {
    console.log(error);
  }
}
