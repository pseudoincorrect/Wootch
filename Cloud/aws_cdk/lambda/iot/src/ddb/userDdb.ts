import { UserModel, UserModelDynamoDb } from "../models/userModel";
import { DynamodbAccess } from "./dynamodb";

/** Control access to dynamoDb for users models*/
export class UserDdb {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  /**
   * Get a user model from dynamodb
   * @param userKey User dynamodb PK
   * @returns a user model or undefined
   */
  async getUser(userKey: string): Promise<UserModel | undefined> {
    const record = await this.ddbAccess.dynamodbGet(userKey);
    return UserModel.fromDynamodb(record as UserModelDynamoDb);
  }

  /**
   * Update the last email sent date in dynamodb
   * @param userModel User Model
   */
  async updateLastEmail(userModel: UserModel) {
    const updateExpression = "set LAST_EMAIL_DATE = :k";

    const expressionAttributeValues = {
      ":k": userModel.LAST_EMAIL_DATE,
    };

    await this.ddbAccess.dynamodbUpdate(
      userModel.PK,
      updateExpression,
      expressionAttributeValues
    );
  }
}
