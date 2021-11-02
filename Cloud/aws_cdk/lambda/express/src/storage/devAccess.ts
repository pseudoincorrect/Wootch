import { DevModel, DevModelDynamoDb } from "../models/devModel";
import { DynamodbAccess } from "../storage/dynamodb";

export interface DevStorage {
  getDev(id: string): Promise<DevModel | undefined>;

  updateDev(devModel: DevModel): Promise<any>;
}

export class DevAccess implements DevStorage {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  async getDev(devKey: string): Promise<DevModel | undefined> {
    const record = await this.ddbAccess.dynamodbGet(devKey);
    console.log(record);
    return DevModel.fromDynamodb(record as DevModelDynamoDb);
  }

  async updateDev(devModel: DevModel): Promise<any> {
    const record = devModel.createDynamodbRecord();

    const updateExpression = "set DEV_USER_KEY = :k";

    const expressionAttributeValues = {
      ":k": devModel.DEV_USER_KEY,
    };

    return this.ddbAccess.dynamodbUpdate(
      devModel.PK,
      updateExpression,
      expressionAttributeValues
    );
  }
}
