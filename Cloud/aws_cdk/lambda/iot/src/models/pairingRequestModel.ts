import * as Joi from "joi";

const PAT_PAIR_KEY = RegExp(/^PAI#[a-zA-Z0-9]*$/);
const PAT_DEV_KEY = RegExp(/^DEV#[a-fA-F0-9]*$/);

/** Validation Schema for pairing request */
const PairingRequestValidationSchema = Joi.object({
  PK: Joi.string().pattern(PAT_PAIR_KEY).uppercase().required(),
  PAIR_CREATION_DATE: Joi.number().required(),
  DEV_KEY: Joi.string().pattern(PAT_DEV_KEY).uppercase().required(),
  ttl: Joi.number().required(),
});

/** Interface used to construct a PairModel */
interface PairModelConf {
  PK: string;
  PAIR_CREATION_DATE: number;
  DEV_KEY: string;
  ttl: number;
}

interface PairModelDynamoDbRecord extends PairModelConf {}

export class PairModel {
  public PK: string;
  public PAIR_CREATION_DATE: number;
  public DEV_KEY: string;
  public ttl: number;

  constructor(conf: PairModelConf) {
    this.PK = conf.PK;
    this.PAIR_CREATION_DATE = conf.PAIR_CREATION_DATE;
    this.DEV_KEY = conf.DEV_KEY;
    this.ttl = conf.ttl;
  }

  /** Factory, create PairModel from DynamoDb Record */
  static fromDynamodb(record: PairModelConf) {
    const { error } = PairingRequestValidationSchema.validate(record);
    if (error) throw error;

    return new PairModel(record);
  }

  /** Factory, create PairModel from PairModelConf*/
  static fromScratch(conf: PairModelConf) {
    const { error } = PairingRequestValidationSchema.validate(conf);
    if (error) throw error;
    return new PairModel(conf);
  }

  /** Factory, create dynamodb record from PairModel*/
  createDynamodbRecord(): PairModelDynamoDbRecord {
    const record: PairModelDynamoDbRecord = {
      PK: this.PK,
      PAIR_CREATION_DATE: this.PAIR_CREATION_DATE,
      DEV_KEY: this.DEV_KEY,
      ttl: this.ttl,
    };

    return record;
  }

  /** return a string view of PairModel */
  toString(): string {
    return JSON.stringify(this.toJson(), null, 2);
  }

  /** return a JSON view of PairModel */
  toJson(): object {
    return {
      PK: this.PK,
      PAIR_CREATION_DATE: this.PAIR_CREATION_DATE,
      DEV_KEY: this.DEV_KEY,
      ttl: this.ttl,
    };
  }
}
