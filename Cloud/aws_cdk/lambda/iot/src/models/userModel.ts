import * as Joi from "joi";

const PAT_USER_KEY = RegExp(/^USR#[A-Z0-9]*$/);
const PAT_IS_ANUM_UPCASE = RegExp(/^[A-F0-9]*$/);

const PREFIX_USER_KEY = "USR#";

/** Validation Schema for UserModel Dynamodb record */
const UserModelValidationSchema = Joi.object({
  PK: Joi.string().pattern(PAT_USER_KEY).uppercase().required(),
  USER_EMAIL: Joi.string().email().required(),
  CREATION_DATE: Joi.number().required(),
  LAST_ONLINE_DATE: Joi.number(),
});

/** Interface used to construct a User Model  */
export interface UserModelConf {
  PK: string;
  USER_EMAIL: string;
  CREATION_DATE: number;
  LAST_ONLINE_DATE?: number;
}

/** Interface used to construct a User Model for DynamoDb */
export interface UserModelDynamoDb extends UserModelConf {}

/** User model to interac uniformly on the app */
export class UserModel {
  public PK: string;
  public USER_EMAIL: string;
  public CREATION_DATE: number;
  public LAST_ONLINE_DATE?: number;

  constructor(conf: UserModelConf) {
    this.PK = conf.PK;
    this.USER_EMAIL = conf.USER_EMAIL;
    this.CREATION_DATE = conf.CREATION_DATE;
    if (conf.LAST_ONLINE_DATE) this.LAST_ONLINE_DATE = conf.LAST_ONLINE_DATE;
  }

  /** Factory, create UserModel from DynamoDb Record */
  static fromDynamodb(record: UserModelDynamoDb) {
    const { error } = UserModelValidationSchema.validate(record);
    if (error) throw error;

    return new UserModel({
      PK: record.PK,
      USER_EMAIL: record.USER_EMAIL,
      CREATION_DATE: record.CREATION_DATE,
      LAST_ONLINE_DATE: record.LAST_ONLINE_DATE,
    });
  }

  /** Factory, create dynamodb record from UserModel*/
  createDynamodbRecord(): UserModelDynamoDb {
    const record: UserModelDynamoDb = {
      PK: this.PK,
      USER_EMAIL: this.USER_EMAIL,
      CREATION_DATE: this.CREATION_DATE,
    };

    if (this.LAST_ONLINE_DATE) {
      record["LAST_ONLINE_DATE"] = this.LAST_ONLINE_DATE;
    }

    const { error } = UserModelValidationSchema.validate(record);
    if (error) throw error;

    return record;
  }

  static getPkFromId(userId: string): string {
    if (userId.length != 32 || !userId.match(PAT_IS_ANUM_UPCASE)) {
      throw new Error("Pairing PK format");
    }
    return `${PREFIX_USER_KEY}${userId}`;
  }

  /** return a string view of UserModel */
  toString(): string {
    return JSON.stringify(this.toJson(), null, 2);
  }

  /** return a JSON view of UserModel */
  toJson(): object {
    return {
      PK: this.PK,
      CREATION_DATE: this.CREATION_DATE,
      userLastOnline: this.LAST_ONLINE_DATE,
    };
  }
}
