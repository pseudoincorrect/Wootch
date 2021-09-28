import * as sinon from "sinon";
import { expect } from "chai";

import * as iot from "../../src/aws/awsIot";
import * as iotH from "../../src/aws/awsIotHelper";
import { assert } from "console";

describe("certIdFromCertArn", function () {
  it("should pass without error and return the correct value", () => {
    const arn =
      "arn:aws:iot:eu-west-1:123123123123:cert/123456789abcdef123456789abcdef123456789abcdef123456789abcdef1234";
    const shortArn = iotH.certIdFromCertArn(arn);
    expect(shortArn).equal(
      "123456789abcdef123456789abcdef123456789abcdef123456789abcdef1234"
    );
  });

  it("should throw an error for incorrect length", () => {
    let arn = "arn:aws:io";
    expect(function () {
      iotH.certIdFromCertArn(arn);
    }).to.throw("Arn format error");
  });

  it("should throw an error for incorrect characters", () => {
    // note the "X" after "cert/"
    const arn =
      "arn:aws:iot:eu-west-1:123123123123:cert/X23456789abcdef123456789abcdef123456789abcdef123456789abcdef1234";
    expect(function () {
      iotH.certIdFromCertArn(arn);
    }).to.throw("Arn format error");
  });
});

describe("getDeviceCertificate", function () {
  afterEach(() => {
    sinon.restore();
  });

  it("should pass without error and return an Arn", async () => {
    const fakeCert = "123456789";
    sinon.replace(iot, "deviceListCertificate", sinon.fake.returns([fakeCert]));
    const cert = await iotH.getDeviceCertificate("wootch_device_123");
    expect(cert).equals(fakeCert);
  });

  it("should pass without error and return null", async () => {
    sinon.replace(iot, "deviceListCertificate", sinon.fake.returns([]));
    const cert = await iotH.getDeviceCertificate("wootch_device_123");
    expect(cert).is.null;
  });

  it("should propagate the error", async () => {
    sinon.replace(
      iot,
      "deviceListCertificate",
      sinon.fake.throws(new Error("httpError"))
    );
    try {
      const cert = await iotH.getDeviceCertificate("wootch_device_123");
    } catch (error) {
      expect(error).to.be.an("Error");
    }
  });
});

// describe("", function () {
//   it("should ", () => {});
// });
