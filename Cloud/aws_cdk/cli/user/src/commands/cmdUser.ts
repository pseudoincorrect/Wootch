import { Args } from "./cmdTypes";
import * as awsUser from "../aws/awsAmplifyAuth";

export async function cmdLogin(argv: Args) {
  await awsUser.signUp();
}
