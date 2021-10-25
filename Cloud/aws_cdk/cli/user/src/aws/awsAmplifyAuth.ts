import Amplify, { Auth } from "aws-amplify";

export async function signUp(username: string, password: string) {
  try {
    const user = await Auth.signIn(username, password);
    const idToken = user.getSignInUserSession().getIdToken().getJwtToken();
    return idToken;
  } catch (error) {
    console.log("error signing up:", error);
  }
}

export async function signOut() {
  return await Auth.signOut();
}
