import { expect } from "chai";
// import rewire from "rewire";
import * as iot from "../../src/aws/awsIot";
import * as iotHelp from "../../src/aws/awsIotHelper";

describe("Device: creation and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    await iot.deviceCreate(devId);
    let exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.true;

    await iot.deviceDelete(devId);
    exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("Certificate: creation and deletion", function () {
  it("Should pass without error", async () => {
    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    let certId = cert.certificateId;
    let exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.true;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.false;
  });
});

describe("Policy: Creation and Deletion", function () {
  it("Should pass without error", async () => {
    await iot.policyIotCreate();
    let exists = await iotHelp.policyIotExist();
    expect(exists).to.be.true;

    await iot.policyIotDelete();
    exists = await iotHelp.policyIotExist();
    expect(exists).to.be.false;
  });
});

describe("Certificate and device: creation, linking and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;
    await iot.deviceCreate(devId);
    exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.true;

    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.true;

    await iot.deviceAttachCertificate(devId, certArn);
    hasCert = await iotHelp.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.true;

    await iot.deviceDetachCertificate(devId, certArn);
    hasCert = await iotHelp.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.false;

    await iot.deviceDelete(devId);
    exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("Certificate, device, policy: creation, linking and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;
    let hasPol = null;
    await iot.deviceCreate(devId);
    exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.true;

    const cert: iot.CreateCertificateOutput = await iot.certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.true;

    await iot.policyIotCreate();
    exists = await iotHelp.policyIotExist();
    expect(exists).to.be.true;

    await iot.certificateAttachPolicy(certArn);
    hasPol = await iotHelp.certificateHasPolicy(certArn);
    expect(hasPol).to.be.true;

    await iot.deviceAttachCertificate(devId, certArn);
    hasCert = await iotHelp.deviceHasCertificate(devId, certArn);
    expect(hasCert).to.be.true;

    await iot.certificateDetachPolicy(certArn);
    hasPol = await iotHelp.certificateHasPolicy(certArn);
    expect(hasPol).to.be.false;

    await iot.policyIotDelete();
    exists = await iotHelp.policyIotExist();
    expect(exists).to.be.false;

    await iot.deviceDetachCertificate(devId, certArn);
    hasCert = await iotHelp.deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await iot.certificateDeactivate(certId);
    await iot.certificateDelete(certId);
    exists = await iotHelp.certificateExist(certId);
    expect(exists).to.be.false;

    await iot.deviceDelete(devId);
    exists = await iotHelp.deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe.only("playAround", function () {
  it("Should pass without error", async () => {
    const arn = "arn:aws:iot:eu-west-1:281859560513:cert/17b20237e3d55789f03ea4015111d3c71332ffc21253b54374ca3c9bf998c7a2";
    console.log(iotHelp.certIdFromCertArn(arn));
  });
});
