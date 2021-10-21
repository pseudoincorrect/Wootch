import * as cdk from "@aws-cdk/core";
import * as lambda from "@aws-cdk/aws-lambda";
import * as cognito from "@aws-cdk/aws-cognito";
import { CdkContext, Utils } from "./utils";
import * as cst from "./constants";
import { v4 as uuidv4 } from "uuid";

const groupsAttributeName = Utils.getEnv("GROUPS_ATTRIBUTE_NAME", "groups");
const groupsAttributeClaimName = "custom:" + groupsAttributeName;
const nodeRuntime: lambda.Runtime = lambda.Runtime.NODEJS_10_X;
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

    const preTokenGenFunction = new lambda.Function(
      this,
      "preTokenGenFunction",
      {
        functionName: stackAndEnv + "preTokenGenFunction",
        runtime: nodeRuntime,
        handler: "index.handler",
        code: lambda.Code.fromAsset("../lambda/cognito/dist/src"),
        environment: {
          GROUPS_ATTRIBUTE_CLAIM_NAME: groupsAttributeClaimName,
        },
      }
    );

    // User pool config
    this.userPool = new cognito.UserPool(this, "UserPool", {
      userPoolName: stackAndEnv + "UserPool",
      lambdaTriggers: { preTokenGenFunction },
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
      customAttributes: {
        organisationID: new cognito.StringAttribute({
          minLen: 3,
          maxLen: 64,
          mutable: true,
        }),
        userID: new cognito.StringAttribute({
          minLen: 3,
          maxLen: 64,
          mutable: true,
        }),
      },
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

    const randomDomain = `wootch-` + uuidv4().toString();

    const cfnUserPoolDomain = new cognito.CfnUserPoolDomain(
      this,
      "userPoolDomain",
      {
        userPoolId: this.userPool.userPoolId,
        domain: randomDomain,
      }
    );

    const signingUiUrl = `https://${randomDomain}/login?response_type=code&client_id=${cfnUserPoolClient.ref}&redirect_uri=${redirectUrl}`;

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
  }
}
