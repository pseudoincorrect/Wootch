import { v4 as uuidv4 } from "uuid";

import * as secrets from "../../secrets/secrets";
import * as cogn from "../aws/awsCognito";
import * as ddb from "../aws/awsDdb";
import { getAnId } from "./cmdHelpers";
import { Args } from "./cmdTypes";

export async function cmdCreateUser(argv: Args) {
  const email = argv.email;
  const password = argv.password;
  let userId = getAnId();
  try {
    await cogn.createUser(email, password, userId);
    await cogn.addUserToGroup(email);
    await ddb.createUser(userId, email);
  } catch (error) {
    console.log(error);
  }
  console.log(`User ID: ${userId}`);

  console.log(
    "[EMAIL] Please use and change your temporary passords on : \n" +
      `${secrets.SIGNING_URL}`
  );
}

export async function cmdDeleteUser(argv: Args) {
  const email = argv.email;
  try {
    await cogn.deleteUser(email);
  } catch (error) {
    console.log(error);
  }
}
