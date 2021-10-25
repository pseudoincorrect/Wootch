import * as Joi from "joi";

import * as pat from "./patternsModel";

/** Validation Schema for DevModel Dynamodb record */
const DevModelDynamoDbValidationSchema = Joi.object({
  PK: Joi.string().pattern(pat.DEV_KEY).uppercase().required(),
  DEV_CREATION_DATE: Joi.date().required(),
  DEV_USR_KEY: Joi.string().pattern(pat.USR_KEY).uppercase(),
  DEV_LAST_ONLINE: Joi.date(),
});

/** Interface used to construct a Device Model on DynamoDb */
export interface DevModelDynamoDb {
  PK: string;
  DEV_CREATION_DATE: string;
  DEV_USR_KEY?: string;
  DEV_LAST_ONLINE_DATE?: string;
}

/** Interface used to construct a DevModel */
export interface DevModelConf {
  devKey: string;
  devCreationDate: string;
  devUsrKey?: string;
  devLastOnlineDate?: string;
}

export class DevModel {
  public devKey: string;
  public devCreationDate: string;
  public devUsrKey?: string;
  public devLastOnlineDate?: string;

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
      devUsrKey: record.DEV_USR_KEY,
      devLastOnlineDate: record.DEV_LAST_ONLINE_DATE,
    });
  }

  /** Factory, create DevModelDynamoDb from DevModel*/
  createDynamodbRecord(): DevModelDynamoDb {
    const record: DevModelDynamoDb = {
      PK: this.devKey,
      DEV_USR_KEY: this.devUsrKey,
      DEV_CREATION_DATE: this.devCreationDate,
      DEV_LAST_ONLINE_DATE: this.devLastOnlineDate,
    };

    if (this.devUsrKey) record["DEV_USR_KEY"] = this.devUsrKey;
    if (this.devLastOnlineDate)
      record["DEV_LAST_ONLINE_DATE"] = this.devLastOnlineDate;

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
