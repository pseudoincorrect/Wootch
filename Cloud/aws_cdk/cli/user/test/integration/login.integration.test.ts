import { expect } from "chai";
import * as fs from "fs";

// import rewire from "rewire";
import * as auth from "../../src/aws/awsAmplifyAuth";
import { amplifyConfigure } from "../../src/aws/awsAmplifyConfig";
import * as secrets from "../../secrets/secrets";
import * as fm from "../../src/utils/filesManipulation";

const username = secrets.MOCK_USERNAME;
const password = secrets.MOCK_PASSWORD;
const credPath = fm.getCredentialsDir();
const dirPath = fm.getLocalTmpDataDirPath();

describe("Local app data paths", function () {
  this.afterEach(async function () {
    fs.rmSync(dirPath, { recursive: true, force: true });
  });

  this.beforeEach(async function () {
    fs.rmSync(dirPath, { recursive: true, force: true });
  });

  it("should pass without error", async () => {
    const dirPath = fm.getLocalTmpDataDirPath();
    expect(fs.existsSync(dirPath)).to.be.false;
    fm.createLocalTmpDataDir();
    expect(fs.existsSync(dirPath)).to.be.true;
  });
});

describe("Credentials path", function () {
  this.afterEach(async function () {
    fs.rmSync(credPath, { recursive: true, force: true });
  });

  this.beforeEach(async function () {
    fs.rmSync(dirPath, { recursive: true, force: true });
  });

  it("should pass without error", async () => {
    expect(fs.existsSync(credPath)).to.be.false;
    fm.createCredentialsDir();
    expect(fs.existsSync(credPath)).to.be.true;
  });
});

describe("signing in sign out", function () {
  this.beforeEach(async function () {
    fs.rmSync(credPath, { recursive: true, force: true });
  });

  this.afterEach(async function () {
    fs.rmSync(credPath, { recursive: true, force: true });
  });

  it("Should pass without error", async () => {
    await amplifyConfigure();
    const token = await auth.signUp(username, password);
    fm.storeCredentials(token);
    let tok = fs.readFileSync(fm.getCredentialsFilePath());
    expect(tok).to.not.be.empty;
    await auth.signOut();
    fm.deleteCredentials();
    expect(fs.existsSync(fm.getCredentialsFilePath())).to.be.false;
  });
});
