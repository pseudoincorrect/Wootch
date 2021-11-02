import { PairModel, PairModelDynamoDbRecord } from "../models/pairingModel";
import { DynamodbAccess } from "./dynamodb";

export interface PairingStorage {
  getPairing(id: string): Promise<PairModel | undefined>;

  deletePairing(id: string): Promise<any>;
}

export class PairingAccess implements PairingStorage {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  async getPairing(pairId: string): Promise<PairModel | undefined> {
    const pk = PairModel.getPkFromId(pairId);
    const record = await this.ddbAccess.dynamodbGet(pk);
    console.log(record);
    return PairModel.fromDynamodb(record as PairModelDynamoDbRecord);
  }

  async deletePairing(pairId: string): Promise<any> {
    const pk = PairModel.getPkFromId(pairId);
    const record = await this.ddbAccess.dynamodbDelete(pk);
    console.log(record);
  }
}
