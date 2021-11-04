import { DevModel, DevModelDynamoDb } from "../models/devModel";
import { DynamodbAccess } from "./dynamodb";

/** Control access to dynamoDb for device models*/
export class DevDdb {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  /**
   * Get a device Model from dynamodb
   * @param devKey device Key
   * @returns  device model or undefined
   */
  async getDev(devKey: string): Promise<DevModel | undefined> {
    const record = await this.ddbAccess.dynamodbGet(devKey);
    console.log(record);
    return DevModel.fromDynamodb(record as DevModelDynamoDb);
  }

  /**
   * Update the UserKey of a device in dynamodb
   * @param devModel device model
   */
  async updateDev(devModel: DevModel) {
    const updateExpression = "set DEV_USER_KEY = :k";

    const expressionAttributeValues = {
      ":k": devModel.DEV_USER_KEY,
    };

    await this.ddbAccess.dynamodbUpdate(
      devModel.PK,
      updateExpression,
      expressionAttributeValues
    );
  }
}
