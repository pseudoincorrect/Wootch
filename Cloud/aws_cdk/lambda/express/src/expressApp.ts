// import aws = require("aws-sdk");
import { App, AppOptions } from "./app";
import { Utils } from "./utils/utils";
import { authGroups } from "./utils/authGroups";

const appOptions: AppOptions = {
  authGroups,
  authorizationHeaderName: Utils.getEnv("AUTHORIZATION_HEADER_NAME"),
  userPoolId: Utils.getEnv("USER_POOL_ID"),
  // cognito: new aws.CognitoIdentityServiceProvider(),
  // forceSignOutHandler: new DynamoDBForcedSignoutHandler(Utils.getEnv("USERS_TABLE_NAME")),
};

export const expressApp = new App(appOptions).expressApp;
