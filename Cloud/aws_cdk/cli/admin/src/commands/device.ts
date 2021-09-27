import { Args } from "./cmdTypes";
import * as iot from "@aws-sdk/client-iot";
import { AWS_REGION, AWS_ACCOUNT_ID } from "../../secrets/awsParams";
import wootchPolicy from "../policies/iotPolicy.json";

const WOOTCH_DEVICE_TYPE = "wootch_device";
const WOOTCH_POLICY_NAME = "wootch_device_policy";

const iotClient = new iot.IoTClient({ region: AWS_REGION });

export interface CreateCertificateOutput {
  certificateArn: string;
  certificateId: string;
  certificatePem: string;
  keyPair: {
    PrivateKey: string;
    PublicKey: string;
  };
}

export interface PolicySearch {
  policyArn: string;
  policyName: string;
}

/**
 * Create a thing device on AWS IOT
 * @param argv cli parameters
 * @returns
 */
export async function cmdCreateDevice(argv: Args) {
  try {
    await deviceCreate(argv.id);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Create a generic device type on AWS IOT
 */
export async function cmdCreateDeviceType() {
  try {
    await deviceCreateType();
  } catch (error) {
    console.log(error);
  }
}

/**
 * Delete a thing on AWS IOT if it exists
 * @param argv cli parameter
 */
export async function cmdDeleteDevice(argv: Args) {
  try {
    await deviceDelete(argv.id);
  } catch (error) {
    console.log(error);
  }
}

/**
 * Search a thing device and print its info
 * @param argv cli parameters
 */
export async function cmdSearchDevice(argv: Args) {
  try {
    const data = await deviceDescribe(argv.id);
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
 * Get a shortened Arn (for display purpose)
 * @param arn AWS Arn address
 * @returns shortned version of this Arn
 */
function shortArn(arn: string): string {
  return "arn/" + arn.slice(40, 45);
}

/**
 * Get a shortened ID (for display purpose)
 * @param id any ID
 * * @returns shortned version of this Arn
 */
function shortId(id: string): string {
  if (id.length < 5) throw new Error("id length");
  return id.slice(0, 5);
}

/**
 * Create a thing device on AWS IOT
 * @param id device ID
 * @returns
 */
async function deviceCreate(id: string) {
  const params: iot.CreateThingCommandInput = {
    thingName: nameFromId(id),
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingCommand(params);
  let data: iot.CreateThingCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    if (error.message.includes("TYPE")) {
      console.log("ERROR : Please create a thing type");
    } else {
      console.log(error);
    }
    throw error;
  }
  console.log(`Created device ${nameFromId(id)}`);
}

/**
 * Create a generic device type on AWS IOT
 */
async function deviceCreateType() {
  const params: iot.CreateThingTypeCommandInput = {
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingTypeCommand(params);
  let data: iot.CreateThingTypeCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Created device type ${WOOTCH_DEVICE_TYPE}`);
}

/**
 * Delete a thing on AWS IOT if it exists
 * @param id device id
 */
async function deviceDelete(id: string) {
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
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Deleted device ${nameFromId(id)}`);
}

/**
 * Search for a thing device on AWS IOT
 * @param id device id
 * @returns AWS IOT Device Data
 */
async function deviceDescribe(id: string): Promise<any> {
  const params: iot.DescribeThingCommandInput = {
    thingName: nameFromId(id),
  };
  const command = new iot.DescribeThingCommand(params);
  let data: iot.DescribeThingCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Found device ${nameFromId(id)}`);
  return data;
}

/**
 * Return whether a AWS IOT device exist
 * @param id device ID
 * @returns if the device exists
 */
async function deviceExist(id: string): Promise<boolean> {
  try {
    await deviceDescribe(id);
  } catch (error) {
    return false;
  }
  return true;
}

/**
 * Return whether a AWS IOT device has a certificate attached
 * @param id device ID
 * @returns if the device has a certificate attached
 */
async function deviceHasCertificate(
  devId: string,
  certArn: string
): Promise<boolean> {
  try {
    const data = await deviceListCertificate(devId);
    if (!data.principals) return false;
    if (data.principals[0] == certArn) return true;
  } catch (error) {
    return false;
  }
  return false;
}

/**
 * Attach a certificate to a device (AWS IOT)
 * @param devId device ID
 * @param certifId certificate ID
 * @returns completion info
 */
async function deviceAttachCertificate(
  devId: string,
  certArn: string
): Promise<any> {
  const params: iot.AttachThingPrincipalCommandInput = {
    thingName: nameFromId(devId),
    principal: certArn,
  };
  const command = new iot.AttachThingPrincipalCommand(params);
  let data: iot.AttachThingPrincipalCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Attached certificate ${shortArn(certArn)}.. to device ${nameFromId(devId)}`
  );
  return data;
}

/**
 * Detach a certificate to a device (AWS IOT)
 * @param devId device ID
 * @param certifId certificate ID
 * @returns completion info
 */
async function deviceDetachCertificate(
  devId: string,
  certArn: string
): Promise<any> {
  const params: iot.DetachThingPrincipalCommandInput = {
    thingName: nameFromId(devId),
    principal: certArn,
  };
  const command = new iot.DetachThingPrincipalCommand(params);
  let data: iot.DetachThingPrincipalCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Detached certificate ${shortArn(certArn)}.. from device ${nameFromId(
      devId
    )}`
  );
  return data;
}

/**
 * list the certificate attached to a device (AWS IOT)
 * @param devId device ID
 * @returns completion info
 */
async function deviceListCertificate(
  devId: string
): Promise<iot.ListThingPrincipalsCommandOutput> {
  const params: iot.ListThingPrincipalsCommandInput = {
    thingName: nameFromId(devId),
  };
  const command = new iot.ListThingPrincipalsCommand(params);
  let data: iot.ListThingPrincipalsCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Retrieved certificate(s) attached to device ${nameFromId(devId)}`
  );
  return data;
}

/**
 * Create a certificate to be attached to a device
 * @returns Certificate Info and associated Keys
 */
async function certificateCreate(): Promise<CreateCertificateOutput> {
  const params: iot.CreateKeysAndCertificateCommandInput = {
    setAsActive: true,
  };
  const command = new iot.CreateKeysAndCertificateCommand(params);
  let data: iot.CreateKeysAndCertificateCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Created certificate ${shortId(data.certificateId!)}..`);
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
async function certificateDelete(id: string): Promise<any> {
  const params: iot.DeleteCertificateCommandInput = {
    certificateId: id,
  };
  const command = new iot.DeleteCertificateCommand(params);
  let data: iot.DeleteCertificateCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Deleted certificate ${shortId(id)}..`);
  return data;
}

/**
 * Search for a AWS IOT certificate
 * @param id certificate ID
 * @returns certificate info
 */
async function certificateDescribe(id: string): Promise<any> {
  const params: iot.DescribeCertificateCommandInput = {
    certificateId: id,
  };
  const command = new iot.DescribeCertificateCommand(params);
  let data: iot.DescribeCertificateCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Found certificate ${shortId(id)}..`);
  return data;
}

/**
 * List the policies attached to a certificate
 * @param certArn certificate Arn
 * @returns list of policies
 */
async function certificateListPolicies(
  certArn: string
): Promise<PolicySearch[]> {
  const params: iot.ListAttachedPoliciesCommandInput = {
    target: certArn,
  };
  const command = new iot.ListAttachedPoliciesCommand(params);
  let data: iot.ListAttachedPoliciesCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Found policies for certificate ${shortArn(certArn)}..`);
  return data.policies as PolicySearch[];
}

/**
 * Deactivate a certificate, usually before deleting it
 * @param id certificate ID
 * @returns deletion data
 */
async function certificateDeactivate(id: string): Promise<any> {
  const params: iot.UpdateCertificateCommandInput = {
    certificateId: id,
    newStatus: "INACTIVE",
  };
  const command = new iot.UpdateCertificateCommand(params);
  let data: iot.UpdateCertificateCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Deactivated certificate ${shortId(id)}..`);
  return data;
}

/**
 * Check if a certificate has the policy attached to it
 * @param certArn certificate Arn
 * @returns whether the certificate has the policy
 */
async function certificateHasPolicy(certArn: string): Promise<boolean> {
  try {
    const pols = await certificateListPolicies(certArn);
    for (let i = 0; i < pols.length; i++) {
      if(pols[i].policyName == WOOTCH_POLICY_NAME) {
        return true
      }
    }
  } catch (error) {
    return false;
  }
  return false;
}


/**
 * Check if a certificate exists
 * @param id certificate ID
 * @returns whether or not a certificate exists
 */
async function certificateExist(id: string): Promise<boolean> {
  try {
    await certificateDescribe(id);
  } catch (error) {
    return false;
  }
  return true;
}

/**
 * Attach the policy to a certificate
 * @param certArn certificate Arn
 * @returns completion info
 */
async function certificateAttachPolicy(certArn: string): Promise<any> {
  const params: iot.AttachPolicyCommandInput = {
    policyName: WOOTCH_POLICY_NAME,
    target: certArn,
  };
  const command = new iot.AttachPolicyCommand(params);
  let data: iot.AttachPolicyCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Attached policy ${WOOTCH_POLICY_NAME} to certificate ${shortArn(certArn)}`
  );
  return data;
}

/**
 * Detach the policy from a certificate
 * @param certArn certificate Arn
 * @returns completion info
 */
async function certificateDetachPolicy(certArn: string): Promise<any> {
  const params: iot.DetachPolicyCommandInput = {
    policyName: WOOTCH_POLICY_NAME,
    target: certArn,
  };
  const command = new iot.DetachPolicyCommand(params);
  let data: iot.DetachPolicyCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Detached policy ${WOOTCH_POLICY_NAME} from certificate ${shortArn(
      certArn
    )}`
  );
  return data;
}

/**
 * Create the policy that enable devices to publish and subscribe to topics
 * */
async function policyIotCreate(): Promise<any> {
  let pol = JSON.stringify(wootchPolicy, null, 2);
  pol = pol.replace(/AWS_ACCOUNT_ID/g, AWS_ACCOUNT_ID);
  const params: iot.CreatePolicyCommandInput = {
    policyName: WOOTCH_POLICY_NAME,
    policyDocument: pol,
  };
  const command = new iot.CreatePolicyCommand(params);
  let data: iot.CreatePolicyCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Created Policy ${WOOTCH_POLICY_NAME}`);
  return data;
}

/**
 * Create the policy that enable devices to publish and subscribe to topics
 * */
async function policyIotDelete(): Promise<any> {
  const params: iot.DeletePolicyCommandInput = {
    policyName: WOOTCH_POLICY_NAME,
  };
  const command = new iot.DeletePolicyCommand(params);
  let data: iot.DeletePolicyCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Deleted Policy ${WOOTCH_POLICY_NAME}`);
  return data;
}
/**
 * Search for an IoT policy
 * */
async function policyIotList(): Promise<PolicySearch[]> {
  const params: iot.ListPoliciesCommandInput = {};
  const command = new iot.ListPoliciesCommand(params);
  let data: iot.ListPoliciesCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Retrieved list of Policies`);
  return data.policies as PolicySearch[];
}

/**
 * Return whether a AWS IOT policy ${WOOTCH_POLICY_NAME} exist
 * @returns if the policy exists
 */
async function policyIotExist(): Promise<boolean> {
  try {
    const data = await policyIotList();
    for (let i = 0; i < data.length; i++) {
      if (data[i].policyName == WOOTCH_POLICY_NAME) {
        return true;
      }
    }
  } catch (error) {
    return false;
  }
  return false;
}
