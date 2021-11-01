import {
  PairModel,
  PairModelDynamoDbRecord,
} from "../models/pairingRequestModel";
import { DynamodbAccess } from "../storage/dynamodb";

export interface PairingRequestStorage {
  getPairingRequest(id: string): Promise<PairModel | undefined>;
}

export class PairingRequestAccess implements PairingRequestStorage {
  private ddbAccess: DynamodbAccess;

  constructor() {
    this.ddbAccess = new DynamodbAccess();
  }

  async getPairingRequest(pairId: string): Promise<PairModel | undefined> {
    const pk = PairModel.getPkFromId(pairId);
    const record = await this.ddbAccess.dynamodbGet(pk);
    console.log(record);
    return PairModel.fromDynamodb(record as PairModelDynamoDbRecord);
  }
}
