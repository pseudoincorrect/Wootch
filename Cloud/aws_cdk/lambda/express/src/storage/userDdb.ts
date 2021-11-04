import { UserModel, UserModelDynamoDb } from "../models/userModel";
import { DynamodbAccess } from "./dynamodb";

/** Control access to dynamoDb for user models*/
export class UserDdb {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  /**
   * Get a user model from dynamodb
   * @param userKey User dynamodb key
   * @returns a user model or undefined
   */
  async getUser(userKey: string): Promise<UserModel | undefined> {
    const record = await this.ddbAccess.dynamodbGet(userKey);
    console.log(record);
    return UserModel.fromDynamodb(record as UserModelDynamoDb);
  }
}
