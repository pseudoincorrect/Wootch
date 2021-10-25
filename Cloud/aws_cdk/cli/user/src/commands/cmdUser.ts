import { Args } from "./cmdTypes";
import * as awsUser from "../aws/awsAmplifyAuth";
import * as fm from "../utils/filesManipulation";

export async function cmdLogin(argv: Args) {
  const username = argv.username;
  const password = argv.password;

  const idToken = await awsUser.signUp(username, password);
  fm.storeCredentials(idToken);
}
