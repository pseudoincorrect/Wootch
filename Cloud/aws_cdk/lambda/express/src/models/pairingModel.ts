import * as Joi from "joi";

import { AppValidationError } from "../utils/appErrors";

const PREFIX_PAIR_KEY = "PAI#";

const PAT_PAIR_KEY = RegExp(/^PAI#[a-zA-Z0-9]*$/);
const PAT_DEV_KEY = RegExp(/^DEV#[a-fA-F0-9]*$/);
const PAT_IS_ANUM_UPCASE = RegExp(/^[A-F0-9]*$/);

/** Validation Schema for pairing request */
const pairingValidationSchema = Joi.object({
  PK: Joi.string().pattern(PAT_PAIR_KEY).uppercase().required(),
  CREATION_DATE: Joi.number().required(),
  DEV_KEY: Joi.string().pattern(PAT_DEV_KEY).uppercase().required(),
  ttl: Joi.number().required(),
});

/** Interface used to construct a PairModel */
interface PairModelConf {
  PK: string;
  CREATION_DATE: number;
  DEV_KEY: string;
  ttl: number;
}

export interface PairModelDynamoDbRecord extends PairModelConf {}

export class PairModel {
  public PK: string;
  public CREATION_DATE: number;
  public DEV_KEY: string;
  public ttl: number;

  constructor(conf: PairModelConf) {
    this.PK = conf.PK;
    this.CREATION_DATE = conf.CREATION_DATE;
    this.DEV_KEY = conf.DEV_KEY;
    this.ttl = conf.ttl;
  }

  /** Factory, create PairModel from DynamoDb Record */
  static fromDynamodb(record: PairModelDynamoDbRecord) {
    const { error } = pairingValidationSchema.validate(record);
    if (error) throw error;

    return new PairModel(record);
  }

  /** Factory, create dynamodb record from PairModel*/
  createDynamodbRecord(): PairModelDynamoDbRecord {
    const record: PairModelDynamoDbRecord = {
      PK: this.PK,
      CREATION_DATE: this.CREATION_DATE,
      DEV_KEY: this.DEV_KEY,
      ttl: this.ttl,
    };

    return record;
  }

  static getPkFromId(pairId: string): string {
    if (pairId.length != 6 || !pairId.match(PAT_IS_ANUM_UPCASE)) {
      throw new AppValidationError("Pairing PK format");
    }
    return `${PREFIX_PAIR_KEY}${pairId}`;
  }

  /** return a string view of PairModel */
  toString(): string {
    return JSON.stringify(this.toJson(), null, 2);
  }

  /** return a JSON view of PairModel */
  toJson(): object {
    return {
      PK: this.PK,
      CREATION_DATE: this.CREATION_DATE,
      DEV_KEY: this.DEV_KEY,
      ttl: this.ttl,
    };
  }
}
