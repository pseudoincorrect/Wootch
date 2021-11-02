import Amplify from "aws-amplify";

import * as secrets from "../../secrets/secrets";

// docs.amplify.aws/lib/auth/start/q/platform/js/#create-new-authentication-resource

export async function amplifyConfigure() {
  Amplify.configure({
    Auth: {
      region: secrets.AWS_REGION,
      userPoolId: secrets.USER_POOL_ID,
      userPoolWebClientId: secrets.USER_POOL_CLIENT_ID,
      mandatorySignIn: true,
      authenticationFlowType: "USER_PASSWORD_AUTH",
    },
  });
}
