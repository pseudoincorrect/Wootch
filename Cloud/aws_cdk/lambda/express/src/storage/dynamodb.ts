import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import * as ddbLib from "@aws-sdk/lib-dynamodb";

import { AppDbError } from "../utils/appErrors";
import { Utils } from "../utils/utils";

/**
 * Class controlling the access of dynamodb
 */
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

  /**
   * Get a record from dynamodb from a primary key
   * @param pk Primary Key
   * @returns a dynamodb record or null
   */
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
      throw new AppDbError("failed get org record from db", err);
    }
    return data.Item;
  }

  /**
   * Put a record in dynamodb
   * @param record dynamodb record (key : value)
   */
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

  /**
   * Update a dynamodb record with provided values
   * @param pk Primary Key
   * @param updateExpression dynamodb update expression
   * @param expressionAttributeValues dynamodb values associated
   * with the update expression
   */
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
      throw new AppDbError("failed put org record in db", err);
    }
  }

  /**
   * Delete a record from dynamodb
   * @param pk Primary Key
   * @returns the deleted record or null
   */
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
      throw new AppDbError("failed get org record from db", err);
    }
    return data.Item;
  }
}
