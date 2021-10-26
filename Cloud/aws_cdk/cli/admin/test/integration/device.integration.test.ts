import { expect } from "chai";

import * as ddb from "../../src/aws/awsDdb";
import * as iot from "../../src/aws/awsIot";
import * as iotHelpers from "../../src/aws/awsIotHelpers";

describe("aws iot, device creation and deletion", function () {
  it("should pass without error", async () => {
    const devId = "123";
    await iot.deviceCreate(devId);
    let exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.true;

    await iot.deviceDelete(devId);
    exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("aws iot, certificate creation and deletion", function () {
  it("should pass without error", async () => {
    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    let certId = cert.certificateId;
    let exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.true;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.false;
  });
});

describe("aws iot, certificate and device: creation, linking and deletion", function () {
  it("should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;

    if (!(await iotHelpers.deviceTypeExist())) {
      await iot.deviceTypeCreate();
    }
    exists = await iotHelpers.deviceTypeExist();
    expect(exists).to.be.true;

    await iot.deviceCreate(devId);
    exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.true;

    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.true;

    await iot.deviceAttachCertificate(devId, certArn);
    hasCert = await iotHelpers.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.true;

    await iot.deviceDetachCertificate(devId, certArn);
    hasCert = await iotHelpers.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.false;

    await iot.deviceDelete(devId);
    exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("aws iot, certificate, device, policy: creation, linking and deletion", function () {
  it("should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;
    let hasPol = null;

    if (!(await iotHelpers.deviceTypeExist())) {
      await iot.deviceTypeCreate();
    }
    exists = await iotHelpers.deviceTypeExist();
    expect(exists).to.be.true;

    await iot.deviceCreate(devId);
    exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.true;

    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.true;

    if (!iotHelpers.policyIotExist()) {
      await iot.policyIotCreate();
    }
    exists = await iotHelpers.policyIotExist();
    expect(exists).to.be.true;

    await iot.certificateAttachPolicy(certArn);
    hasPol = await iotHelpers.certificateHasPolicy(certArn);
    expect(hasPol).to.be.true;

    await iot.deviceAttachCertificate(devId, certArn);
    hasCert = await iotHelpers.deviceHasCertificate(devId, certArn);
    expect(hasCert).to.be.true;

    await iot.certificateDetachPolicy(certArn);
    hasPol = await iotHelpers.certificateHasPolicy(certArn);
    expect(hasPol).to.be.false;

    await iot.deviceDetachCertificate(devId, certArn);
    hasCert = await iotHelpers.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelpers.certificateExist(certId);
    expect(exists).to.be.false;

    await iot.deviceDelete(devId);
    exists = await iotHelpers.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("dynamodb, device creation and deletion dynamodb", function () {
  it("should pass without error", async () => {
    const id = "3871BED249D24A019196E88F4E384325";
    await ddb.createDevice(id);
    let devModel = await ddb.getDevice(id);
    expect(devModel).not.to.be.undefined;

    await ddb.deleteDevice(id);
    devModel = await ddb.getDevice(id);
    expect(devModel).to.be.undefined;
  });
});

describe.only("dynamodb, user creation and deletion dynamodb", function () {
  it("should pass without error", async () => {
    const id = "3871BED249D24A019196E88F4E384325";
    await ddb.createUser(id);
    let userModel = await ddb.getUser(id);
    expect(userModel).not.to.be.undefined;

    await ddb.deleteUser(id);
    userModel = await ddb.getUser(id);
    expect(userModel).to.be.undefined;
  });
});

describe("playaround", function () {
  it("should pass without error", async () => {});
});
