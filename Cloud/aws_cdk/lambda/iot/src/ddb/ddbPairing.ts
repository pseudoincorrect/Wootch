import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddb from "@aws-sdk/lib-dynamodb";

import * as utils from "../utils/utils";
import { PairModel } from "../models/PairingModel";

const region = utils.getEnv("MAIN_AWS_REGION");
const mainTableName = utils.getEnv("MAIN_DATA_TABLE");

const client = new DynamoDBClient({ region });
const ddbClient = ddb.DynamoDBDocument.from(client);

/**
 * Create a pairing request and put it in dynamodb
 * @param id Device Id
 */
export async function createPairing(devId: string, secret: string) {
  const pairKey = utils.pairKeyFromSecret(secret);
  const devKey = utils.devKeyFromDevId(devId);
  const pairModel = PairModel.fromScratch({
    PK: pairKey,
    DEV_KEY: devKey,
    CREATION_DATE: new Date().getTime(),
    ttl: new Date().getTime() + 3 * 60 * 1000, // now + 3 minutes (in ms)
  });
  const record = pairModel.createDynamodbRecord();

  try {
    await ddbClient.put({
      TableName: mainTableName,
      Item: record,
    });
  } catch (error) {
    console.log(error);
    throw error;
  }
}
