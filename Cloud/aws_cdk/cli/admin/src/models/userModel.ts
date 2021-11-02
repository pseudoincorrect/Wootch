import * as Joi from "joi";

import * as pat from "./patternsModel";

/** Validation Schema for UserModel Dynamodb record */
const UserModelDynamoDbValidationSchema = Joi.object({
  PK: Joi.string().pattern(pat.USER_KEY).uppercase().required(),
  USER_EMAIL: Joi.string().email().required(),
  CREATION_DATE: Joi.number().required(),
  LAST_ONLINE_DATE: Joi.number(),
});

/** Validation Schema for UserModel Dynamodb record */
const UserModelConfValidationSchema = Joi.object({
  userKey: Joi.string().pattern(pat.USER_KEY).uppercase().required(),
  userEmail: Joi.string().email().required(),
  creationDate: Joi.number().required(),
  lastOnlineDate: Joi.number(),
});

/** Interface used to construct a User Model on DynamoDb */
export interface UserModelDynamoDb {
  PK: string;
  USER_EMAIL: string;
  CREATION_DATE: number;
  LAST_ONLINE_DATE?: number;
}

/** Interface used to construct a UserModel */
export interface UserModelConf {
  userKey: string;
  userEmail: string;
  creationDate: number;
  lastOnlineDate?: number;
}

export class UserModel {
  public userKey: string;
  public userEmail: string;
  public creationDate: number;
  public lastOnlineDate?: number;

  constructor(conf: UserModelConf) {
    this.userKey = conf.userKey;
    this.userEmail = conf.userEmail;
    this.creationDate = conf.creationDate;
    if (conf.lastOnlineDate) this.lastOnlineDate = conf.lastOnlineDate;
  }

  /** Factory, create UserModel from DynamoDb Record */
  static fromDynamodb(record: UserModelDynamoDb) {
    const { error } = UserModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return new UserModel({
      userKey: record.PK,
      userEmail: record.USER_EMAIL,
      creationDate: record.CREATION_DATE,
      lastOnlineDate: record.LAST_ONLINE_DATE,
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
      USER_EMAIL: this.userEmail,
      CREATION_DATE: this.creationDate,
    };

    if (this.lastOnlineDate) {
      record["LAST_ONLINE_DATE"] = this.lastOnlineDate;
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
      creationDate: this.creationDate,
      userLastOnline: this.lastOnlineDate,
    };
  }
}
