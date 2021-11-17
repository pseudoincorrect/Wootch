import { DevDdb } from "./ddb/devDdb";
import { PairingDdb } from "./ddb/pairingDdb";
import { UserDdb } from "./ddb/userDdb";
import { DevModel } from "./models/devModel";
import * as sEmail from "./ses/activityEmail";
import * as utils from "./utils/utils";

const PairingPattern = RegExp(
  /^WootchDev\/device\/WOOTCH_DEV_[a-zA-Z0-9]+\/pairing\/request/
);
const activityPattern = RegExp(
  /^WootchDev\/device\/WOOTCH_DEV_[a-zA-Z0-9]+\/data\/activity/
);

interface PairingEvent {
  secret: string;
  topic: string;
  clientid: string;
}

interface ActivityEvent {
  watchLvl: string;
  maxAcc: number;
  accThreshold: number;
  maxRot: number;
  rotThreshold: number;
  topic: string;
  clientid: string;
}

const pairingDdb = new PairingDdb();
const userDdb = new UserDdb();
const devDdb = new DevDdb();
const receiverAdress = utils.getEnv("RECEIVER_ADDRESS");
/**
 * Entry point of the lambda function
 * @param event MQTT message and metadata
 * @param context lambda function context
 */
async function iotReceive(event: any, context: any) {
  console.log(JSON.stringify(event, null, 2));
  if (activityPattern.test(event.topic)) {
    await processActivityEvent(event as ActivityEvent);
  } else if (PairingPattern.test(event.topic)) {
    await processPairingEvent(event as PairingEvent);
  }
}

/**
 * Process an MQTT activity message, by alerting the user by email
 * @param event data/metadata of the activity MQTT message
 */
async function processActivityEvent(event: ActivityEvent) {
  console.log("Activity message");
  // Get Model of the device associated with this MQTT message
  const devId = utils.devIdFromThingName(event.clientid);
  const devKey = DevModel.getPkFromId(devId);
  let devModel;
  try {
    devModel = await devDdb.getDev(devKey);
  } catch (error) {
    console.log(error);
    throw new Error("Could not get DevModel (ddb)");
  }
  if (!devModel?.DEV_USER_KEY) {
    console.log("No user paired to this device");
    return;
  }
  // Get user associated with this device
  const userKey = devModel.DEV_USER_KEY;
  let userModel;
  try {
    userModel = await userDdb.getUser(userKey);
  } catch (error) {
    console.log(error);
    throw new Error("Could not get UserModel (ddb)");
  }
  // Check whether an email has been received recently
  const lastEmail = userModel!.LAST_EMAIL_DATE;
  const nowMs = new Date().getTime();
  if (lastEmail != undefined) {
    if (nowMs - lastEmail < 60 * 1000) {
      return;
    }
  }
  // Send him a message
  const userEmail = userModel!.USER_EMAIL;
  console.log(`Sending email to ${receiverAdress}`);
  await sEmail.sendEmail(receiverAdress);
  // Update last email sent date
  userModel!.LAST_EMAIL_DATE = nowMs;
  await userDdb.updateLastEmail(userModel!);
}

/**
 * Create a pairing request to enable a user to assciate himself
 * with a device
 * @param event data/metadata of the pairing MQTT message
 */
async function processPairingEvent(event: PairingEvent) {
  console.log("Pairing message");
  const secret = event.secret;
  const devId = utils.devIdFromThingName(event.clientid);
  // Add a pairing request in the Db
  try {
    await pairingDdb.putPairing(devId, secret);
    console.log(`Pairing request ${secret} created`);
  } catch (error) {
    throw new Error("Could not create pairing request");
  }
}

export const handler = iotReceive;
