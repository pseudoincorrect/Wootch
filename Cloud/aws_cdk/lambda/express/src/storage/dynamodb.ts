import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddbLib from "@aws-sdk/lib-dynamodb";

import { AppDbError } from "../utils/appErrors";
import { Utils } from "../utils/utils";

export class DynamodbAccess {
  private ddbClient: DynamoDBClient;
  private ddb: ddbLib.DynamoDBDocumentClient;
  private tableName: string;

  constructor() {
    const region = Utils.getEnv("AWS_APP_REGION");
    this.tableName = Utils.getEnv("APP_TABLE_NAME");
    this.ddbClient = new DynamoDBClient({ region });
    this.ddb = ddbLib.DynamoDBDocumentClient.from(this.ddbClient);
  }

  async dynamodbGet(pk: string): Promise<any | null> {
    let data: any;
    try {
      data = await this.ddb.send(
        new ddbLib.GetCommand({
          TableName: this.tableName,
          Key: {
            PK: pk,
          },
        })
      );
    } catch (err: any) {
      throw new AppDbError("failed get org record from db", err);
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
      throw new AppDbError("failed put org record in db", err);
    }
  }
}
