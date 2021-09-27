import rewire from "rewire";
import * as chai from "chai";
import { CreateCertificateOutput } from "src/commands/device";
const expect = chai.expect;

const app = rewire("../../src/commands/device.ts");

// device
const deviceCreate = app.__get__("deviceCreate");
const deviceDelete = app.__get__("deviceDelete");
const deviceListCertificate = app.__get__("deviceListCertificate");
const deviceAttachCertificate = app.__get__("deviceAttachCertificate");
const deviceDetachCertificate = app.__get__("deviceDetachCertificate");
const deviceExist = app.__get__("deviceExist");
const deviceHasCertificate = app.__get__("deviceHasCertificate");
// certificate
const certificateCreate = app.__get__("certificateCreate");
const certificateDelete = app.__get__("certificateDelete");
const certificateDeactivate = app.__get__("certificateDeactivate");
const certificateDescribe = app.__get__("certificateDescribe");
const certificateExist = app.__get__("certificateExist");
const certificateHasPolicy = app.__get__("certificateHasPolicy");
const certificateAttachPolicy = app.__get__("certificateAttachPolicy");
const certificateDetachPolicy = app.__get__("certificateDetachPolicy");
const certificateListPolicies = app.__get__("certificateListPolicies");
// policy
const policyIotCreate = app.__get__("policyIotCreate");
const policyIotExist = app.__get__("policyIotExist");
const policyIotDelete = app.__get__("policyIotDelete");

describe("Device: creation and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    await deviceCreate(devId);
    let exists = await deviceExist(devId);
    expect(exists).to.be.true;

    await deviceDelete(devId);
    exists = await deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("Certificate: creation and deletion", function () {
  it("Should pass without error", async () => {
    const cert: CreateCertificateOutput = await certificateCreate();
    let certId = cert.certificateId;
    let exists = await certificateExist(certId);
    expect(exists).to.be.true;

    await certificateDeactivate(certId);
    await certificateDelete(certId);
    exists = await certificateExist(certId);
    expect(exists).to.be.false;
  });
});

describe("Policy: Creation and Deletion", function () {
  it("Should pass without error", async () => {
    await policyIotCreate();
    let exists = await policyIotExist();
    expect(exists).to.be.true;

    await policyIotDelete();
    exists = await policyIotExist();
    expect(exists).to.be.false;
  });
});

describe("Certificate and device: creation, linking and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;
    await deviceCreate(devId);
    exists = await deviceExist(devId);
    expect(exists).to.be.true;

    const cert: CreateCertificateOutput = await certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await certificateExist(certId);
    expect(exists).to.be.true;

    await deviceAttachCertificate(devId, certArn);
    hasCert = await deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.true;

    await deviceDetachCertificate(devId, certArn);
    hasCert = await deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await certificateDeactivate(certId);
    await certificateDelete(certId);
    exists = await certificateExist(certId);
    expect(exists).to.be.false;

    await deviceDelete(devId);
    exists = await deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe.only("Certificate, device, policy: creation, linking and deletion", function () {
  it("Should pass without error", async () => {
    const devId = "123";
    let exists = null;
    let hasCert = null;
    let hasPol = null;
    await deviceCreate(devId);
    exists = await deviceExist(devId);
    expect(exists).to.be.true;

    const cert: CreateCertificateOutput = await certificateCreate();
    const certId = cert.certificateId;
    const certArn = cert.certificateArn;
    exists = await certificateExist(certId);
    expect(exists).to.be.true;

    await policyIotCreate();
    exists = await policyIotExist();
    expect(exists).to.be.true;

    await certificateAttachPolicy(certArn);
    hasPol = await certificateHasPolicy(certArn);
    expect(hasPol).to.be.true;

    await deviceAttachCertificate(devId, certArn);
    hasCert = await deviceHasCertificate(devId, certArn);
    expect(hasCert).to.be.true;

    await certificateDetachPolicy(certArn);
    hasPol = await certificateHasPolicy(certArn);
    expect(hasPol).to.be.false;

    await policyIotDelete();
    exists = await policyIotExist();
    expect(exists).to.be.false;

    await deviceDetachCertificate(devId, certArn);
    hasCert = await deviceHasCertificate(devId, certId);
    expect(hasCert).to.be.false;

    await certificateDeactivate(certId);
    await certificateDelete(certId);
    exists = await certificateExist(certId);
    expect(exists).to.be.false;

    await deviceDelete(devId);
    exists = await deviceExist(devId);
    expect(exists).to.be.false;
  });
});

describe("playAround", function () {
  it("Should pass without error", async () => {
    console.log("Play around test !")
  });
});
