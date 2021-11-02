import { expect } from "chai";
import * as fs from "fs";

import * as secrets from "../../secrets/secrets";
import * as fm from "../../src/utils/filesManipulation";
import * as pairingRequest from "../../src/requests/pairingRequest";

const credPath = fm.getCredentialsDir();
const dirPath = fm.getLocalTmpDataDirPath();

describe("Local app data paths", function () {
  it("should pass without error", async () => {
    // pairingRequest.pairingRequest()
  });
});
