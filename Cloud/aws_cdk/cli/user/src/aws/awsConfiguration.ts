import Amplify, { Auth } from "aws-amplify";

// docs.amplify.aws/lib/auth/start/q/platform/js/#create-new-authentication-resource

export async function amplifyConfigure() {
  Amplify.configure({
    Auth: {
      // REQUIRED - Amazon Cognito Region
      region: "eu-west-1",

      // OPTIONAL - Amazon Cognito User Pool ID
      userPoolId: "eu-west-1_qg7YeDjla",

      userPoolWebClientId: "4cfn2lv62cu585l3i2j3trlbpa",

      // OPTIONAL - Enforce user authentication prior to accessing AWS resources or not
      mandatorySignIn: true,

      // OPTIONAL - Manually set the authentication flow type. Default is 'USER_SRP_AUTH'
      authenticationFlowType: "USER_PASSWORD_AUTH",
    },
  });
}
