import { DevAccess } from "./ddb/devAccess";
import { PairingAccess } from "./ddb/pairingAccess";
import { UserAccess } from "./ddb/userAccess";
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
  threshold: number;
  topic: string;
  clientid: string;
}

const pairAccess = new PairingAccess();
const userAccess = new UserAccess();
const devAccess = new DevAccess();

async function iotReceive(event: any, context: any) {
  console.log(JSON.stringify(event, null, 2));

  if (activityPattern.test(event.topic)) {
    await processActivityEvent(event as ActivityEvent);
  } else if (PairingPattern.test(event.topic)) {
    await processPairingEvent(event as PairingEvent);
  }
}

async function processActivityEvent(event: ActivityEvent) {
  console.log("Activity message");
  const devId = utils.devIdFromThingName(event.clientid);
  const devKey = DevModel.getPkFromId(devId);
  let devModel;
  try {
    devModel = await devAccess.getDev(devKey);
  } catch (error) {
    console.log(error);
    throw new Error("Could not get DevModel (ddb)");
  }
  if (!devModel?.DEV_USER_KEY) {
    console.log("No user paired to this device");
    return;
  }
  const userKey = devModel.DEV_USER_KEY;
  let userModel;
  try {
    userModel = await userAccess.getUser(userKey);
  } catch (error) {
    console.log(error);
    throw new Error("Could not get UserModel (ddb)");
  }
  const userEmail = userModel!.USER_EMAIL;
  console.log(`Sending email to ${userEmail}`);
  await sEmail.sendEmail("maximeclement6@gmail.com");
}

async function processPairingEvent(event: PairingEvent) {
  console.log("Pairing message");
  const secret = event.secret;
  const devId = utils.devIdFromThingName(event.clientid);
  try {
    await pairAccess.putPairing(devId, secret);
    console.log(`Pairing request ${secret} created`);
  } catch (error) {
    throw new Error("Could not create pairing request");
  }
}

export const handler = iotReceive;
