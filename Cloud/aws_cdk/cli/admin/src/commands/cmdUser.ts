import * as cogn from "../aws/awsCognito";
import { Args } from "./cmdTypes";
import * as secrets from "../../secrets/awsParams";

export async function cmdCreateUser(argv: Args) {
  const email = argv.email;
  const password = argv.password;
  try {
    await cogn.createUser(email, password);
    await cogn.addUserToGroup(email);
  } catch (error) {
    console.log(error);
  }
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
