import * as Joi from "joi";

import * as pat from "./patternsModel";

/** Validation Schema for DevModel Dynamodb record */
const DevModelDynamoDbValidationSchema = Joi.object({
  PK: Joi.string().pattern(pat.DEV_KEY).uppercase().required(),
  DEV_CREATION_DATE: Joi.number().required(),
  DEV_USER_KEY: Joi.string().pattern(pat.USER_KEY).uppercase(),
  DEV_LAST_ONLINE_DATE: Joi.number(),
});

/** Validation Schema for DevModel Dynamodb record */
const DevModelConfValidationSchema = Joi.object({
  devKey: Joi.string().pattern(pat.DEV_KEY).uppercase().required(),
  devCreationDate: Joi.number().required(),
  devUsrKey: Joi.string().pattern(pat.USER_KEY).uppercase(),
  devLastOnlineDate: Joi.number(),
});

/** Interface used to construct a Device Model on DynamoDb */
export interface DevModelDynamoDb {
  PK: string;
  DEV_CREATION_DATE: number;
  DEV_USER_KEY?: string;
  DEV_LAST_ONLINE_DATE?: number;
}

/** Interface used to construct a DevModel */
export interface DevModelConf {
  devKey: string;
  devCreationDate: number;
  devUsrKey?: string;
  devLastOnlineDate?: number;
}

export class DevModel {
  public devKey: string;
  public devCreationDate: number;
  public devUsrKey?: string;
  public devLastOnlineDate?: number;

  constructor(conf: DevModelConf) {
    this.devKey = conf.devKey;
    this.devCreationDate = conf.devCreationDate;
    if (conf.devLastOnlineDate) this.devLastOnlineDate = conf.devLastOnlineDate;
    if (conf.devUsrKey) this.devUsrKey = conf.devUsrKey;
  }

  /** Factory, create DevModel from DynamoDb Record */
  static fromDynamodb(record: DevModelDynamoDb) {
    const { error } = DevModelDynamoDbValidationSchema.validate(record);
    if (error) throw error;

    return new DevModel({
      devKey: record.PK,
      devCreationDate: record.DEV_CREATION_DATE,
      devUsrKey: record.DEV_USER_KEY,
      devLastOnlineDate: record.DEV_LAST_ONLINE_DATE,
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
      DEV_CREATION_DATE: this.devCreationDate,
      DEV_LAST_ONLINE_DATE: this.devLastOnlineDate,
    };

    if (this.devUsrKey) {
      record["DEV_USER_KEY"] = this.devUsrKey;
    }
    if (this.devLastOnlineDate) {
      record["DEV_LAST_ONLINE_DATE"] = this.devLastOnlineDate;
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
      devCreationDate: this.devCreationDate,
      devUsrKey: this.devUsrKey,
      devLastOnline: this.devLastOnlineDate,
    };
  }
}
