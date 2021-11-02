import { UserModel, UserModelDynamoDb } from "../models/userModel";
import { DynamodbAccess } from "../storage/dynamodb";

export interface UserStorage {
  getUser(id: string): Promise<UserModel | undefined>;
}

export class UserAccess implements UserStorage {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  async getUser(userKey: string): Promise<UserModel | undefined> {
    const record = await this.ddbAccess.dynamodbGet(userKey);
    console.log(record);
    return UserModel.fromDynamodb(record as UserModelDynamoDb);
  }
}
