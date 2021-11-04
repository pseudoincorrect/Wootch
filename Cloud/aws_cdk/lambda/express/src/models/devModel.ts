import * as Joi from "joi";

const PAT_DEV_KEY = RegExp(/^DEV#[A-F0-9]*$/);
const PAT_USER_KEY = RegExp(/^USR#[A-Z0-9]*$/);

/** Validation Schema for DevModel Dynamodb record */
const DevModelValidationSchema = Joi.object({
  PK: Joi.string().pattern(PAT_DEV_KEY).uppercase().required(),
  CREATION_DATE: Joi.number().required(),
  DEV_USER_KEY: Joi.string().pattern(PAT_USER_KEY).uppercase(),
  LAST_ONLINE_DATE: Joi.number(),
});

/** Interface used to construct a Device Model on DynamoDb */
export interface DevModelConf {
  PK: string;
  CREATION_DATE: number;
  DEV_USER_KEY?: string;
  LAST_ONLINE_DATE?: number;
}

export interface DevModelDynamoDb extends DevModelConf {}

/** Model englobing a device for its storage in db */
export class DevModel {
  public PK: string;
  public CREATION_DATE: number;
  public DEV_USER_KEY?: string;
  public LAST_ONLINE_DATE?: number;

  constructor(conf: DevModelConf) {
    this.PK = conf.PK;
    this.CREATION_DATE = conf.CREATION_DATE;
    if (conf.LAST_ONLINE_DATE) this.LAST_ONLINE_DATE = conf.LAST_ONLINE_DATE;
    if (conf.DEV_USER_KEY) this.DEV_USER_KEY = conf.DEV_USER_KEY;
  }

  /** Factory, create DevModel from DynamoDb Record */
  static fromDynamodb(record: DevModelDynamoDb): DevModel {
    const { error } = DevModelValidationSchema.validate(record);
    if (error) throw error;

    return new DevModel({
      PK: record.PK,
      CREATION_DATE: record.CREATION_DATE,
      DEV_USER_KEY: record.DEV_USER_KEY,
      LAST_ONLINE_DATE: record.LAST_ONLINE_DATE,
    });
  }

  /** Factory, create dynamodb record from DevModel*/
  createDynamodbRecord(): DevModelDynamoDb {
    const record: DevModelDynamoDb = {
      PK: this.PK,
      DEV_USER_KEY: this.DEV_USER_KEY,
      CREATION_DATE: this.CREATION_DATE,
      LAST_ONLINE_DATE: this.LAST_ONLINE_DATE,
    };

    if (this.DEV_USER_KEY) {
      record["DEV_USER_KEY"] = this.DEV_USER_KEY;
    }
    if (this.LAST_ONLINE_DATE) {
      record["LAST_ONLINE_DATE"] = this.LAST_ONLINE_DATE;
    }

    const { error } = DevModelValidationSchema.validate(record);
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
      PK: this.PK,
      CREATION_DATE: this.CREATION_DATE,
      DEV_USER_KEY: this.DEV_USER_KEY,
      devLastOnline: this.LAST_ONLINE_DATE,
    };
  }
}
