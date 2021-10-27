import * as ddbPairing from "./ddb/ddbPairing";
import * as utils from "./utils/utils";
const pairingRequestPattern = RegExp(
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

async function iotReceive(event: any, context: any) {
  console.log(JSON.stringify(event, null, 2));

  if (activityPattern.test(event.topic)) {
    await processActivityEvent(event as ActivityEvent);
  } else if (pairingRequestPattern.test(event.topic)) {
    await processPairingEvent(event as PairingEvent);
  }
}

async function processActivityEvent(event: ActivityEvent) {
  console.log("Activity message");
}

async function processPairingEvent(event: PairingEvent) {
  console.log("Pairing message");
  const secret = event.secret;
  const devId = utils.devIdFromThingName(event.clientid);
  await ddbPairing.createPairingRequest(devId, secret);
}

export const handler = iotReceive;
