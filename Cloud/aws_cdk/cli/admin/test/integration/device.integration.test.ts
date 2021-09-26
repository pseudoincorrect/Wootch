import rewire from "rewire";
import * as chai from "chai";
const expect = chai.expect;

const app = rewire("../../src/commands/device.ts");

var createDevice = app.__get__("createDevice");
var deleteDevice = app.__get__("deleteDevice");
var describeDevice = app.__get__("describeDevice");

describe("device creation and deletion", function () {
  it("should create a device and delete it", async () => {
    const devId = "123";
    await createDevice(devId);
    const dev = await describeDevice(devId);
    expect(dev.defaultClientId).to.equal("wootch_dev_123");
    console.log(dev);
    await deleteDevice(devId);
    let error = null;
    try {
      await describeDevice(devId);
    } catch (err) {
      error = err;
    }
    expect(error).to.be.an("Error");
  });
});

describe("certificate creation and deletion", function () {
  it("should create a certificate and delete it", async () => {
    console.log("hey");
    
  });
});

describe("certificate and device creation, linking and deletion", function () {
  it("should create a certificate and delete it", async () => {
    console.log("hey");
  });
});