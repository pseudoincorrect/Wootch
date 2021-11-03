import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddbLib from "@aws-sdk/lib-dynamodb";

import * as utils from "../utils/utils";

export class DynamodbAccess {
  private ddbClient: DynamoDBClient;
  private ddb: ddbLib.DynamoDBDocumentClient;
  private tableName: string;

  constructor() {
    const region = utils.getEnv("MAIN_AWS_REGION");
    this.tableName = utils.getEnv("MAIN_DATA_TABLE");
    this.ddbClient = new DynamoDBClient({ region });
    this.ddb = ddbLib.DynamoDBDocumentClient.from(this.ddbClient);
  }

  async dynamodbGet(pk: string): Promise<any | null> {
    let data: any;
    try {
      data = await this.ddb.send(
        new ddbLib.GetCommand({
          TableName: this.tableName,
          Key: { PK: pk },
        })
      );
    } catch (err: any) {
      console.log(err);
      throw new Error("failed get org record from db");
    }
    return data.Item;
  }

  async dynamodbPut(record: any) {
    try {
      await this.ddb.send(
        new ddbLib.PutCommand({
          TableName: this.tableName,
          Item: record,
        })
      );
    } catch (err: any) {
      console.log(err);
      throw new Error("failed put org record in db");
    }
  }

  async dynamodbUpdate(
    pk: string,
    updateExpression: string,
    expressionAttributeValues: { [key: string]: any }
  ) {
    try {
      await this.ddb.send(
        new ddbLib.UpdateCommand({
          TableName: this.tableName,
          Key: { PK: pk },
          UpdateExpression: updateExpression,
          ExpressionAttributeValues: expressionAttributeValues,
        })
      );
    } catch (err: any) {
      console.log(err);
      throw new Error("failed put org record in db");
    }
  }

  async dynamodbDelete(pk: string): Promise<any | null> {
    let data: any;
    try {
      data = await this.ddb.send(
        new ddbLib.DeleteCommand({
          TableName: this.tableName,
          Key: { PK: pk },
        })
      );
    } catch (err: any) {
      console.log(err);
      throw new Error("failed get org record from db");
    }
    return data.Item;
  }
}
