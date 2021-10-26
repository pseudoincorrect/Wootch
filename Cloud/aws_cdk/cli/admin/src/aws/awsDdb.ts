import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddb from "@aws-sdk/lib-dynamodb";

import * as secrets from "../../secrets/awsParams";
import { DevModel, DevModelDynamoDb } from "../models/devModel";
import { UserModel, UserModelDynamoDb } from "../models/userModel";
import * as awsHelpers from "./awsHelpers";

const marshallOptions = {
  convertEmptyValues: false,
  removeUndefinedValues: true,
  convertClassInstanceToMap: false,
};

const unmarshallOptions = {
  wrapNumbers: false,
};

const translateConfig = { marshallOptions, unmarshallOptions };
const client = new DynamoDBClient({ region: secrets.AWS_REGION });
const ddbClient = ddb.DynamoDBDocument.from(client, translateConfig);

/**
 * Create a device model and Put it in Dynamodb
 * @param id Device ID
 */
export async function createDevice(id: string) {
  const devKey = awsHelpers.devKeyFromId(id);
  const devModel = DevModel.fromScratch({
    devKey,
    devCreationDate: new Date().getTime(),
  });
  const record = devModel.createDynamodbRecord();

  try {
    await ddbClient.put({
      TableName: secrets.DDB_TABLE_NAME,
      Item: record,
    });
  } catch (error) {
    console.log(error);
    throw error;
  }
}

/**
 * Get a device from Dynamodb
 * @param id device ID
 * @returns Device model or undefined
 */
export async function getDevice(id: string): Promise<DevModel | undefined> {
  const devKey = awsHelpers.devKeyFromId(id);
  try {
    const record = await ddbClient.get({
      TableName: secrets.DDB_TABLE_NAME,
      Key: {
        PK: devKey,
      },
    });
    if (record.Item != undefined) {
      return DevModel.fromDynamodb(record.Item as DevModelDynamoDb);
    } else return undefined;
  } catch (error) {
    console.log(error);
    throw error;
  }
}

/**
 * Delete a device from DynamoDb
 * @param id Device ID
 */
export async function deleteDevice(id: string) {
  const devKey = awsHelpers.devKeyFromId(id);
  try {
    await ddbClient.delete({
      TableName: secrets.DDB_TABLE_NAME,
      Key: {
        PK: devKey,
      },
    });
  } catch (error) {
    console.log(error);
    throw error;
  }
}

/**
 * Create a User Model and put it in dynamodb
 * @param id User ID
 */
export async function createUser(id: string, email: string) {
  const userKey = awsHelpers.userKeyFromId(id);
  const userModel = UserModel.fromScratch({
    userKey,
    userEmail: email,
    userCreationDate: new Date().getTime(),
  });
  const record = userModel.createDynamodbRecord();

  try {
    await ddbClient.put({
      TableName: secrets.DDB_TABLE_NAME,
      Item: record,
    });
  } catch (error) {
    console.log(error);
    throw error;
  }
}

/**
 * Get a User from dynamodb
 * @param id User ID
 * @returns User Model or Undefined
 */
export async function getUserById(id: string): Promise<UserModel | undefined> {
  const userKey = awsHelpers.userKeyFromId(id);
  try {
    const record = await ddbClient.get({
      TableName: secrets.DDB_TABLE_NAME,
      Key: {
        PK: userKey,
      },
    });
    if (record.Item != undefined) {
      return UserModel.fromDynamodb(record.Item as UserModelDynamoDb);
    } else return undefined;
  } catch (error) {
    console.log(error);
    throw error;
  }
}

/**
 * Delete a User in Dynamydb
 * @param id User ID
 */
export async function deleteUserById(id: string) {
  const userKey = awsHelpers.userKeyFromId(id);
  try {
    await ddbClient.delete({
      TableName: secrets.DDB_TABLE_NAME,
      Key: {
        PK: userKey,
      },
    });
  } catch (error) {
    console.log(error);
    throw error;
  }
}
