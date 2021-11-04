import { DevModel } from "../models/devModel";
import { PairModel } from "../models/pairingModel";
import { DynamodbAccess } from "./dynamodb";

/** Control access to dynamoDb for pairing models*/
export class PairingDdb {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  /**
   * Create a pairing request and put it in dynamodb
   * @param id Device Id
   */
  async putPairing(devId: string, secret: string): Promise<any> {
    const pairKey = PairModel.getPkFromId(secret);
    const devKey = DevModel.getPkFromId(devId);
    const pairModel = PairModel.fromScratch({
      PK: pairKey,
      DEV_KEY: devKey,
      CREATION_DATE: new Date().getTime(),
      ttl: new Date().getTime() + 3 * 60 * 1000, // now + 3 minutes (in ms)
    });
    const record = pairModel.createDynamodbRecord();
    return this.ddbAccess.dynamodbPut(record);
  }
}
