import { expect } from "chai";
import * as utils from "../../src/utils/utils";

describe("devIdFromThingName", function () {
  it("it should pass without error", () => {
    const thingName =
      utils.wootchThingNamePrefix + "8DFF33C0E67B40D5A125D5F0C7DF9E67";
    const devId = utils.devIdFromThingName(thingName);
    const expectedDevId = "8DFF33C0E67B40D5A125D5F0C7DF9E67";
    expect(devId).to.be.equal(expectedDevId);
  });
});

describe("devKeyFromDevId", function () {
  it("it should pass without error", () => {
    const devId = "8DFF33C0E67B40D5A125D5F0C7DF9E67";
    const devKey = utils.devKeyFromDevId(devId);
    const expectedDevKey =
      utils.wootchDevKeyPrefix + "8DFF33C0E67B40D5A125D5F0C7DF9E67";
    expect(devKey).to.be.equal(expectedDevKey);
  });
});

describe("pairKeyFromSecret", function () {
  it("it should pass without error", () => {
    const secret = "8DFF33";
    const pairKey = utils.pairKeyFromSecret(secret);
    const expectedPairKey = utils.wootchPairKeyPrefix + "8DFF33";
    expect(pairKey).to.be.equal(expectedPairKey);
  });
});
