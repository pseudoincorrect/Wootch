import * as cogn from "@aws-sdk/client-cognito-identity-provider";

import * as secrets from "../../secrets/awsParams";

const userPoolId = secrets.USER_USER_POOL_ID;
const groupName = secrets.USER_GROUP_NAME;

const cognClient = new cogn.CognitoIdentityProviderClient({
  region: secrets.AWS_REGION,
});

/**
 * Create a user in the cognito pool
 * @param email
 * @param tempPassword
 */
export async function createUser(email: string, tempPassword: string) {
  const cmd = new cogn.AdminCreateUserCommand({
    UserPoolId: userPoolId,
    Username: email,
    DesiredDeliveryMediums: ["EMAIL"],
    ForceAliasCreation: false,
    TemporaryPassword: tempPassword,
    UserAttributes: [],
    ClientMetadata: {},
    MessageAction: "SUPPRESS",
    ValidationData: [],
  });

  let res: cogn.AdminCreateUserCommandOutput;
  try {
    res = await cognClient.send(cmd);
    console.log(JSON.stringify(res, null, 2));
  } catch (error: any) {
    if (error.name == "UsernameExistsException") {
      console.log(`User ${email} already exists`);
    } else {
      console.log(error);
    }
    throw error;
  }
  console.log(`user ${email} created`);
}

/**
 * Assign a user to a the regular user group
 * @param params user creation parameters
 */
export async function addUserToGroup(email: string) {
  const cmd = new cogn.AdminAddUserToGroupCommand({
    UserPoolId: userPoolId,
    Username: email,
    GroupName: groupName,
  });

  let res: cogn.AdminAddUserToGroupCommandOutput;
  try {
    res = await cognClient.send(cmd);
    console.log(JSON.stringify(res, null, 2));
  } catch (error: any) {
    if (error.name == "ResourceNotFoundException") {
      console.log(`group ${groupName} does not exist`);
    } else {
      console.log(error);
    }
    throw error;
  }
  console.log(`user ${email} added to group ${groupName}`);
}

/**
 * Delete a user by mail address from cognito pool
 * @param params deletion parameters
 */
export async function deleteUser(email: string) {
  const cmdDisable = new cogn.AdminDisableUserCommand({
    UserPoolId: userPoolId,
    Username: email,
  });

  let resDisable: cogn.AdminDisableProviderForUserCommandOutput;
  try {
    resDisable = await cognClient.send(cmdDisable);
    console.log(JSON.stringify(resDisable, null, 2));
  } catch (error) {
    console.log(error);
    throw error;
  }

  const cmdDelete = new cogn.AdminDeleteUserCommand({
    UserPoolId: userPoolId,
    Username: email,
  });

  let resDelete: cogn.AdminDisableProviderForUserCommandOutput;
  try {
    resDelete = await cognClient.send(cmdDelete);
    console.log(JSON.stringify(resDelete, null, 2));
  } catch (error) {
    console.log(error);
    throw error;
  }
}
