import * as iot from "@aws-sdk/client-iot";

import { AWS_ACCOUNT_ID, AWS_REGION } from "../../secrets/awsParams";
import wootchPolicy from "../policies/iotPolicy.json";
import * as awsHelpers from "./awsHelper";

export const WOOTCH_DEVICE_TYPE = "wootch_device";
export const WOOTCH_POLICY_NAME = "wootch_device_policy";

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
 * @param id device ID
 * @returns promise of completion
 */
export async function deviceCreate(id: string) {
  const params: iot.CreateThingCommandInput = {
    thingName: awsHelpers.nameFromId(id),
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.CreateThingCommand(params);
  let data: iot.CreateThingCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Created device ${awsHelpers.nameFromId(id)}`);
}

/**
 * Create a generic device type on AWS IOT
 * @returns promise of completion
 */
export async function deviceTypeCreate() {
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
export async function deviceDelete(id: string) {
  const params: iot.DeleteThingCommandInput = {
    thingName: awsHelpers.nameFromId(id),
  };
  const command = new iot.DeleteThingCommand(params);
  let data: iot.DeleteThingCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Deleted device ${awsHelpers.nameFromId(id)}`);
}

/**
 * Search for a thing device on AWS IOT
 * @param id device id
 * @returns promise AWS IOT Device Data, Throw an error if not found
 */
export async function deviceDescribe(id: string): Promise<any> {
  const params: iot.DescribeThingCommandInput = {
    thingName: awsHelpers.nameFromId(id),
  };
  const command = new iot.DescribeThingCommand(params);
  let data: iot.DescribeThingCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Found device ${awsHelpers.nameFromId(id)}`);
  return data;
}

/**
 * Search for the thing device Type on AWS IOT
 * @returns promise AWS IOT Device Type Data, Throw an error if not found
 */
export async function deviceTypeDescribe(): Promise<any> {
  const params: iot.DescribeThingTypeCommandInput = {
    thingTypeName: WOOTCH_DEVICE_TYPE,
  };
  const command = new iot.DescribeThingTypeCommand(params);
  let data: iot.DescribeThingTypeCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error: any) {
    throw error;
  }
  console.log(`Found device type ${WOOTCH_DEVICE_TYPE}`);
  return data;
}

/**
 * Attach a certificate to a device (AWS IOT)
 * @param devId device ID
 * @param certifId certificate ID
 * @returns promise of completion
 */
export async function deviceAttachCertificate(
  devId: string,
  certArn: string
): Promise<any> {
  const params: iot.AttachThingPrincipalCommandInput = {
    thingName: awsHelpers.nameFromId(devId),
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
    `Attached certificate ${awsHelpers.shortArn(
      certArn
    )}.. to device ${awsHelpers.nameFromId(devId)}`
  );
  return data;
}

/**
 * Detach a certificate to a device (AWS IOT)
 * @param devId device ID
 * @param certifId certificate ID
 * @returns promise of completion
 */
export async function deviceDetachCertificate(
  devId: string,
  certArn: string
): Promise<any> {
  const params: iot.DetachThingPrincipalCommandInput = {
    thingName: awsHelpers.nameFromId(devId),
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
    `Detached certificate ${awsHelpers.shortArn(
      certArn
    )}.. from device ${awsHelpers.nameFromId(devId)}`
  );
  return data;
}

/**
 * list the certificate attached to a device (AWS IOT)
 * @param devId device ID
 * @returns promise of certificate attached to a device
 */
export async function deviceListCertificate(devId: string): Promise<string[]> {
  const params: iot.ListThingPrincipalsCommandInput = {
    thingName: awsHelpers.nameFromId(devId),
  };
  const command = new iot.ListThingPrincipalsCommand(params);
  let data: iot.ListThingPrincipalsCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(
    `Retrieved certificate(s) attached to device ${awsHelpers.nameFromId(
      devId
    )}`
  );
  return data.principals!;
}

/**
 * Create a certificate to be attached to a device
 * @returns Certificate Info and associated Keys
 */
export async function certificateCreate(): Promise<CreateCertificateOutput> {
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
  console.log(
    `Created certificate ${awsHelpers.shortId(data.certificateId!)}..`
  );
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
 * @returns promise of completion
 */
export async function certificateDelete(id: string): Promise<any> {
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
  console.log(`Deleted certificate ${awsHelpers.shortId(id)}..`);
  return data;
}

/**
 * Search for a AWS IOT certificate
 * @param id certificate ID
 * @returns certificate info, Throw an error if not found
 */
export async function certificateDescribe(
  id: string
): Promise<iot.CertificateDescription> {
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
  console.log(`Found certificate ${awsHelpers.shortId(id)}..`);
  return data.certificateDescription!;
}

/**
 * List the policies attached to a certificate
 * @param certArn certificate Arn
 * @returns promise list of policies attached to a certificate
 */
export async function certificateListPolicies(
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
  console.log(
    `Found policies for certificate ${awsHelpers.shortArn(certArn)}..`
  );
  return data.policies as PolicySearch[];
}

/**
 * Deactivate a certificate, usually before deleting it
 * @param id certificate ID
 * @returns promise of completion
 */
export async function certificateDeactivate(id: string): Promise<any> {
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
  console.log(`Deactivated certificate ${awsHelpers.shortId(id)}..`);
  return data;
}

/**
 * Attach the policy to a certificate
 * @param certArn certificate Arn
 * @returns promise of completion
 */
export async function certificateAttachPolicy(certArn: string): Promise<any> {
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
    `Attached policy ${WOOTCH_POLICY_NAME} to certificate ${awsHelpers.shortArn(
      certArn
    )}`
  );
  return data;
}

/**
 * Detach the policy from a certificate
 * @param certArn certificate Arn
 * @returns completion info
 */
export async function certificateDetachPolicy(certArn: string): Promise<any> {
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
    `Detached policy ${WOOTCH_POLICY_NAME} from certificate ${awsHelpers.shortArn(
      certArn
    )}`
  );
  return data;
}

/**
 * Create the policy that enable devices to publish and subscribe to topics
 * @returns promise of completion
 * */
export async function policyIotCreate(): Promise<any> {
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
 * @returns promise of completion
 * */
export async function policyIotDelete(): Promise<any> {
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
 * Retrieve the list of IoT policies
 * @returns promise of a list of policies
 * */
export async function policyIotList(): Promise<PolicySearch[]> {
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
 * Search for an IoT policy
 * @returns promise of a policy info
 * */
export async function getPolicy(): Promise<iot.GetPolicyCommandOutput> {
  const params: iot.GetPolicyCommandInput = {
    policyName: WOOTCH_POLICY_NAME,
  };
  const command = new iot.GetPolicyCommand(params);
  let data: iot.GetPolicyCommandOutput;
  try {
    data = await iotClient.send(command);
  } catch (error) {
    throw error;
  }
  console.log(`Retrieved list of Policies`);
  return data;
}
