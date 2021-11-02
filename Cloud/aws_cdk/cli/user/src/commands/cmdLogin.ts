import { amplifyConfigure } from "../aws/awsAmplifyConfig";
import * as awsUser from "../aws/awsAmplifyAuth";
import * as fm from "../utils/filesManipulation";
import { Args } from "./cmdTypes";

export async function cmdLogin(argv: Args) {
  await amplifyConfigure();

  const username = argv.email;
  const password = argv.password;

  const idToken = await awsUser.signUp(username, password);

  if (idToken) {
    fm.storeCredentials(idToken);
  }

  console.log(`ID Token : ${idToken}`);
}
