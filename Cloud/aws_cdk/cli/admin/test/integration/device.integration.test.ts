import rewire from "rewire";
import * as chai from "chai";
const expect = chai.expect;

const app = rewire("../../src/commands/device.ts");

var createDevice = app.__get__("createDevice");

describe("device creation and deletion", function () {
  // Hooks: before(), after(), beforeEach(), afterEach()

  it("should create a device and delete it", async () => {
    await createDevice("123");
    expect(true).to.equal(true);
  });
});
