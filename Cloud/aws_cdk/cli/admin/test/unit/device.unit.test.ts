import rewire from "rewire";
import * as chai from "chai";
const expect = chai.expect;

const app = rewire("../../src/commands/device.ts");

var nameFromId = app.__get__("nameFromId"); 

describe("device", function () {
  // Hooks: before(), after(), beforeEach(), afterEach()

  it("it should create a device name", async () => {
    const tryIt = nameFromId("123");
    expect(tryIt).to.equal("wootch_dev_123");
  });

  it("it should create a device name with uppercase after wootch_dev_", async () => {
    const tryIt = nameFromId("abc");
    expect(tryIt).to.equal("wootch_dev_ABC");
  });
});
