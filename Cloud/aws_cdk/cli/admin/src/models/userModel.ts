import * as Joi from "joi";

import * as pat from "./patternsModel";

/** Validation Schema for UserModel Dynamodb record */
const UserModelDynamoDbValidationSchema = Joi.object({
  PK: Joi.string().pattern(pat.USER_KEY).uppercase().required(),
  USER_CREATION_DATE: Joi.number().required(),
  USER_LAST_ONLINE_DATE: Joi.number(),
});

/** Validation Schema for UserModel Dynamodb record */
const UserModelConfValidationSchema = Joi.object({
  userKey: Joi.string().pattern(pat.USER_KEY).uppercase().required(),
  userCreationDate: Joi.number().required(),
  userLastOnlineDate: Joi.number(),
});

/** Interface used to construct a User Model on DynamoDb */
export interface UserModelDynamoDb {
  PK: string;
  USER_CREATION_DATE: number;
  USER_LAST_ONLINE_DATE?: number;
}

/** Interface used to construct a UserModel */
export interface UserModelConf {
  userKey: string;
  userCreationDate: number;
  userLastOnlineDate?: number;
}

export class UserModel {
  public userKey: string;
  public userCreationDate: number;
  public userLastOnlineDate?: number;

  constructor(conf: UserModelConf) {
    this.userKey = conf.userKey;
    this.userCreationDate = conf.userCreationDate;
    if (conf.userLastOnlineDate)
      this.userLastOnlineDate = conf.userLastOnlineDate;
  }

  /** Factory, create UserModel from DynamoDb Record */
  static fromDynamodb(record: UserModelDynamoDb) {
    const { error } = UserModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return new UserModel({
      userKey: record.PK,
      userCreationDate: record.USER_CREATION_DATE,
      userLastOnlineDate: record.USER_LAST_ONLINE_DATE,
    });
  }

  /** Factory, create UserModel from UserModelConf*/
  static fromScratch(conf: UserModelConf) {
    const { error } = UserModelConfValidationSchema.validate(conf);
    if (error) throw error;
    return new UserModel(conf);
  }

  /** Factory, create dynamodb record from UserModel*/
  createDynamodbRecord(): UserModelDynamoDb {
    const record: UserModelDynamoDb = {
      PK: this.userKey,
      USER_CREATION_DATE: this.userCreationDate,
      USER_LAST_ONLINE_DATE: this.userLastOnlineDate,
    };

    if (this.userLastOnlineDate) {
      record["USER_LAST_ONLINE_DATE"] = this.userLastOnlineDate;
    }

    const { error } = UserModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return record;
  }

  /** return a string view of UserModel */
  toString(): string {
    return JSON.stringify(this.toJson(), null, 2);
  }

  /** return a JSON view of UserModel */
  toJson(): object {
    return {
      userKey: this.userKey,
      userCreationDate: this.userCreationDate,
      userLastOnline: this.userLastOnlineDate,
    };
  }
}
