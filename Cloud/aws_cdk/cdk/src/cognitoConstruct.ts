import * as cognito from "@aws-cdk/aws-cognito";
import * as lambda from "@aws-cdk/aws-lambda";
import * as cdk from "@aws-cdk/core";

import * as secrets from "../secrets/secrets";
import * as cst from "./constants";
import { CdkContext, Utils } from "./utils";

const clientDomain = secrets.WOOTCH_COGNITO_POOL_ID;

const groupsAttributeName = Utils.getEnv("GROUPS_ATTRIBUTE_NAME", "groups");
const redirectUrl = "https://github.com/pseudoincorrect/Wootch";

export interface CognitoConstructProps {
  cdkContext: CdkContext;
}

export class CognitoConstruct extends cdk.Construct {
  public userPool: cognito.UserPool;

  constructor(scope: cdk.Construct, id: string, props: CognitoConstructProps) {
    super(scope, id);

    const env = props.cdkContext.env;
    const stackAndEnv = props.cdkContext.stackName + env;

    // User pool config
    this.userPool = new cognito.UserPool(this, "UserPool", {
      userPoolName: stackAndEnv + "UserPool",
      lambdaTriggers: {},
      selfSignUpEnabled: false,
      userVerification: {
        emailSubject: "Verify your email for Wootch !",
        emailBody:
          "Thanks for signing up to Wootch ! Your verification code is {####}",
      },
      userInvitation: {
        emailSubject: "Invite to join Wootch !",
        emailBody:
          "Hello {username}, you have been invited to join Wootch ! Your temporary password is {####}",
      },
      signInAliases: { email: true },
      autoVerify: { email: true },
      standardAttributes: {
        fullname: {
          required: true,
          mutable: false,
        },
      },
      // customAttributes: {
      //   userID: new cognito.StringAttribute({
      //     minLen: 3,
      //     maxLen: 64,
      //     mutable: false,
      //   }),
      // },
      passwordPolicy: {
        minLength: 8,
        requireLowercase: true,
        requireUppercase: true,
        requireDigits: true,
        requireSymbols: false,
        tempPasswordValidity: cdk.Duration.days(365),
      },
      accountRecovery: cognito.AccountRecovery.EMAIL_ONLY,
      signInCaseSensitive: false,
    });

    const userPoolCfn = this.userPool.node.defaultChild as cognito.CfnUserPool;
    userPoolCfn.userPoolAddOns = { advancedSecurityMode: "ENFORCED" };
    userPoolCfn.schema = [
      {
        name: groupsAttributeName,
        attributeDataType: "String",
        mutable: true,
        required: false,
        stringAttributeConstraints: {
          maxLength: "2000",
        },
      },
      {
        name: "userId",
        attributeDataType: "String",
        mutable: false,
        required: false,
        stringAttributeConstraints: {
          maxLength: "128",
        },
      },
    ];

    if (Utils.isEnvDev(env))
      userPoolCfn.applyRemovalPolicy(cdk.RemovalPolicy.DESTROY);
    else userPoolCfn.applyRemovalPolicy(cdk.RemovalPolicy.RETAIN);

    new cognito.CfnUserPoolGroup(this, "AdminsGroup", {
      groupName: cst.adminsGroupName,
      userPoolId: this.userPool.userPoolId,
    });

    new cognito.CfnUserPoolGroup(this, "UsersGroup", {
      groupName: cst.usersGroupName,
      userPoolId: this.userPool.userPoolId,
    });

    const cfnUserPoolClient = new cognito.CfnUserPoolClient(
      this,
      "CognitoAppClient",
      {
        userPoolId: this.userPool.userPoolId,
        clientName: "mobileAppClient",
        // accessTokenValidity: 60 * 60 * 24, // 1 day in seconds
        // idTokenValidity: 60 * 60 * 24,     // 1 day in seconds
        allowedOAuthFlows: ["implicit"],
        allowedOAuthScopes: ["email", "openid"],
        supportedIdentityProviders: ["COGNITO"],
        explicitAuthFlows: [
          "ALLOW_REFRESH_TOKEN_AUTH",
          "ALLOW_USER_PASSWORD_AUTH",
        ],
        callbackUrLs: [redirectUrl],
        logoutUrLs: [redirectUrl],
      }
    );

    const cfnUserPoolDomain = new cognito.CfnUserPoolDomain(
      this,
      "userPoolDomain",
      {
        userPoolId: this.userPool.userPoolId,
        domain: clientDomain,
      }
    );

    const signingUiUrl = `https://${clientDomain}/login?response_type=code&client_id=${cfnUserPoolClient.ref}&redirect_uri=${redirectUrl}`;

    new cdk.CfnOutput(this, "UserPoolSigningPage", {
      description: "User Pool Signing UI URL",
      value: signingUiUrl,
    });

    new cdk.CfnOutput(this, "UserPoolDomain", {
      description: "UserPool Domain",
      value: cfnUserPoolDomain.domain,
    });

    new cdk.CfnOutput(this, "UserPoolIdOutput", {
      description: "UserPool ID",
      value: this.userPool.userPoolId,
    });

    new cdk.CfnOutput(this, "AppClientIdOutput", {
      description: "App Client ID",
      value: cfnUserPoolClient.ref,
    });

    new cdk.CfnOutput(this, "AppClientDomainOutput", {
      description: "App Client Domain",
      value: clientDomain,
    });
  }
}
