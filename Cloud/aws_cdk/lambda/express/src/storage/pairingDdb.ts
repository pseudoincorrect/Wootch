import { PairModel, PairModelDynamoDbRecord } from "../models/pairingModel";
import { DynamodbAccess } from "./dynamodb";

/** Control access to dynamoDb for device models*/
export class PairingDdb {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  /**
   * Get a pairing request model from dynamodb
   * @param pairId ID of pairing request
   * @returns a pairing request model or undefined
   */
  async getPairing(pairId: string): Promise<PairModel | undefined> {
    const pk = PairModel.getPkFromId(pairId);
    const record = await this.ddbAccess.dynamodbGet(pk);
    console.log(record);
    return PairModel.fromDynamodb(record as PairModelDynamoDbRecord);
  }

  /**
   * Delete a pairing request from dynamodb
   * @param pairId ID of a pairing request
   */
  async deletePairing(pairId: string) {
    const pk = PairModel.getPkFromId(pairId);
    const record = await this.ddbAccess.dynamodbDelete(pk);
    console.log(record);
  }
}
