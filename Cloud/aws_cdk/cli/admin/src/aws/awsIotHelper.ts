import * as iot from "../aws/awsIot";

/**
 * return the ID of a certificate from its Arn
 * @param certArn certificate ARn
 * @returns certificate ID
 */
export function certIdFromCertArn(certArn: string) {
  return certArn.slice(40);
}

/**
 * get the certificate of a device if any
 * @param devId device ID
 * @returns certificate Arn
 */
export async function getDeviceCertificate(
  devId: string
): Promise<string | null> {
  let principals = await iot.deviceListCertificate(devId);
  if (principals.length == 0) {
    return null;
  }
  return principals[0];
}

/**
 * Return whether a AWS IOT device exist
 * @param devId device ID
 * @returns if the device exists
 */
export async function deviceExist(devId: string): Promise<boolean> {
  try {
    await iot.deviceDescribe(devId);
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
export async function deviceHasCertificate(
  devId: string,
  certArn: string
): Promise<boolean> {
  try {
    const principals = await iot.deviceListCertificate(devId);
    if (!principals) return false;
    if (principals[0] == certArn) return true;
  } catch (error) {
    return false;
  }
  return false;
}

/**
 * Check if a certificate has the policy attached to it
 * @param certArn certificate Arn
 * @returns whether the certificate has the policy
 */
export async function certificateHasPolicy(certArn: string): Promise<boolean> {
  try {
    const pols = await iot.certificateListPolicies(certArn);
    for (let i = 0; i < pols.length; i++) {
      if (pols[i].policyName == iot.WOOTCH_POLICY_NAME) {
        return true;
      }
    }
  } catch (error) {
    return false;
  }
  return false;
}

/**
 * Check if a certificate exists
 * @param certId certificate ID
 * @returns whether or not a certificate exists
 */
export async function certificateExist(certId: string): Promise<boolean> {
  try {
    await iot.certificateDescribe(certId);
  } catch (error) {
    return false;
  }
  return true;
}

/**
 * Return whether a AWS IOT policy ${WOOTCH_POLICY_NAME} exist
 * @returns if the policy exists
 */
export async function policyIotExist(): Promise<boolean> {
  try {
    const data = await iot.policyIotList();
    for (let i = 0; i < data.length; i++) {
      if (data[i].policyName == iot.WOOTCH_POLICY_NAME) {
        return true;
      }
    }
  } catch (error) {
    return false;
  }
  return false;
}