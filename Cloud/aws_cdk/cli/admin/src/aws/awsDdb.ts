import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddb from "@aws-sdk/lib-dynamodb";

import * as awsHelpers from "./awsHelper";
import * as secrets from "../../secrets/awsParams";

const client = new DynamoDBClient({ region: secrets.AWS_REGION });
const ddbClient = ddb.DynamoDBDocument.from(client);

export async function createDevice(devId: string) {
  const devKey = awsHelpers.getDevKeyFromId(devId);
  await ddbClient.put({
    TableName: secrets.DDB_TABLE_NAME,
    Item: {
      primaryKey: devKey,
      creationDate: new Date().getTime,
      userKey: "",
    },
  });
}

export async function deleteDevice() {}

export async function createUser() {}

export async function deleteUser() {}
