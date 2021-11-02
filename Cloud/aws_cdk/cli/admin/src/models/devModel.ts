import * as Joi from "joi";

import * as pat from "./patternsModel";

/** Validation Schema for DevModel Dynamodb record */
const DevModelDynamoDbValidationSchema = Joi.object({
  PK: Joi.string().pattern(pat.DEV_KEY).uppercase().required(),
  CREATION_DATE: Joi.number().required(),
  DEV_USER_KEY: Joi.string().pattern(pat.USER_KEY).uppercase(),
  LAST_ONLINE_DATE: Joi.number(),
});

/** Validation Schema for DevModel configuration */
const DevModelConfValidationSchema = Joi.object({
  devKey: Joi.string().pattern(pat.DEV_KEY).uppercase().required(),
  creationDate: Joi.number().required(),
  devUsrKey: Joi.string().pattern(pat.USER_KEY).uppercase(),
  lastOnlineDate: Joi.number(),
});

/** Interface used to construct a Device Model on DynamoDb */
export interface DevModelDynamoDb {
  PK: string;
  CREATION_DATE: number;
  DEV_USER_KEY?: string;
  LAST_ONLINE_DATE?: number;
}

/** Interface used to construct a DevModel */
export interface DevModelConf {
  devKey: string;
  creationDate: number;
  devUsrKey?: string;
  lastOnlineDate?: number;
}

export class DevModel {
  public devKey: string;
  public creationDate: number;
  public devUsrKey?: string;
  public lastOnlineDate?: number;

  constructor(conf: DevModelConf) {
    this.devKey = conf.devKey;
    this.creationDate = conf.creationDate;
    if (conf.lastOnlineDate) this.lastOnlineDate = conf.lastOnlineDate;
    if (conf.devUsrKey) this.devUsrKey = conf.devUsrKey;
  }

  /** Factory, create DevModel from DynamoDb Record */
  static fromDynamodb(record: DevModelDynamoDb) {
    const { error } = DevModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return new DevModel({
      devKey: record.PK,
      creationDate: record.CREATION_DATE,
      devUsrKey: record.DEV_USER_KEY,
      lastOnlineDate: record.LAST_ONLINE_DATE,
    });
  }

  /** Factory, create DevModel from DevModelConf*/
  static fromScratch(conf: DevModelConf) {
    const { error } = DevModelConfValidationSchema.validate(conf);
    if (error) throw error;
    return new DevModel(conf);
  }

  /** Factory, create dynamodb record from DevModel*/
  createDynamodbRecord(): DevModelDynamoDb {
    const record: DevModelDynamoDb = {
      PK: this.devKey,
      DEV_USER_KEY: this.devUsrKey,
      CREATION_DATE: this.creationDate,
      LAST_ONLINE_DATE: this.lastOnlineDate,
    };

    if (this.devUsrKey) {
      record["DEV_USER_KEY"] = this.devUsrKey;
    }
    if (this.lastOnlineDate) {
      record["LAST_ONLINE_DATE"] = this.lastOnlineDate;
    }

    const { error } = DevModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return record;
  }

  /** return a string view of DevModel */
  toString(): string {
    return JSON.stringify(this.toJson(), null, 2);
  }

  /** return a JSON view of DevModel */
  toJson(): object {
    return {
      devKey: this.devKey,
      creationDate: this.creationDate,
      devUsrKey: this.devUsrKey,
      devLastOnline: this.lastOnlineDate,
    };
  }
}
