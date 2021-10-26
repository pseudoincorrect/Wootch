import { v4 as uuidv4 } from "uuid";

import * as awsHelpers from "../aws/awsHelpers";
import * as iot from "../aws/awsIot";
import * as ddb from "../aws/awsDdb";
import * as iotHelp from "../aws/awsIotHelpers";
import { Args } from "./cmdTypes";
import { getAnId } from "./cmdHelpers";

/**
 * Create a thing device on AWS IOT
 */
export async function cmdDeviceCreate() {
  const devId = getAnId();
  try {
    if (await iotHelp.deviceExist(devId)) {
      console.log(`device ${devId} already exists`);
      return;
    }
    await iot.deviceCreate(devId);
    if (!(await iotHelp.deviceTypeExist())) {
      await iot.deviceTypeCreate();
    }
    if (!(await iotHelp.policyIotExist())) {
      await iot.policyIotCreate();
    }
    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    console.log(
      "AWS Thing Name (ID) = " + awsHelpers.getThingNameFromId(devId)
    );
    console.log("certificate Arn: \n" + cert.certificateArn + "\n");
    console.log("certificate ID: \n" + cert.certificateId + "\n");
    console.log("certificatePem: \n" + cert.certificatePem + "\n");
    console.log("PrivateKey: \n" + cert.keyPair.PrivateKey + "\n");
    console.log("PublicKey: \n" + cert.keyPair.PublicKey + "\n");

    const certArn = cert.certificateArn;
    await iot.deviceAttachCertificate(devId, certArn);
    await iot.certificateAttachPolicy(certArn);
    await iot.deviceAttachCertificate(devId, certArn);
    await ddb.createDevice(devId);
    console.log(`Device ID: ${devId}`);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Delete a thing on AWS IOT if it exists
 * @param argv cli parameter, argv.id: device ID
 */
export async function cmdDeviceDelete(argv: Args) {
  const devId = argv.id;
  try {
    if (!(await iotHelp.deviceExist(devId))) {
      console.log(`device ${devId} does not exists`);
      return;
    }
    const certArn = await iotHelp.getDeviceCertificate(devId);
    if (!certArn) {
      await iot.deviceDelete(devId);
      return;
    }
    const certId = iotHelp.certIdFromCertArn(certArn);
    await iot.certificateDetachPolicy(certArn);
    await iot.deviceDetachCertificate(devId, certArn);
    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    await iot.deviceDelete(devId);
    await ddb.deleteDevice(devId);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Search a thing device and print its info
 * @param argv cli parameters, argv.id: device ID
 */
export async function cmdDeviceSearch(argv: Args) {
  try {
    const devId = argv.id;
    let data = await iot.deviceDescribe(devId);
    console.log(JSON.stringify(data, null, 2));
    data = await ddb.getDevice(devId);
    console.log(devId);
  } catch (error) {
    console.log(error);
  }
}

export async function cmdCreateDeviceId() {
  let d = uuidv4().replace(/-/g, "").toUpperCase();
  console.log("ID: " + d);
}
