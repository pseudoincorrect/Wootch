import * as cdk from "@aws-cdk/core";
import * as aws from "aws-sdk";
import { CloudFormation } from "aws-sdk";
import apigateway = require("@aws-cdk/aws-apigateway");
import { URL } from "url";

export interface CdkContext {
  stackName: string;
  env: string;
  useCustomDomain: boolean;
  domainHostedZone: string;
  certificateArn: string;
  domainName: string;
}

export class Utils {
  static getCdkContext(app: cdk.App): CdkContext {
    let env = app.node.tryGetContext("config");
    if (!env)
      throw new Error(
        "Context variable missing on CDK command. Pass in as `-c config=XXX`"
      );
    let unparsedEnv = app.node.tryGetContext(env);
    return {
      stackName: unparsedEnv["stackName"],
      env: unparsedEnv["env"],
      useCustomDomain: unparsedEnv["useCustomDomain"] == "true",
      domainHostedZone: unparsedEnv["domainHostedZone"],
      certificateArn: unparsedEnv["certificateArn"],
      domainName: unparsedEnv["domainName"],
    };
  }

  static async getStackOutputs(
    stackName: string,
    stackRegion: string
  ): Promise<CloudFormation.Output[]> {
    aws.config.region = stackRegion;
    const cfn = new aws.CloudFormation();
    const result = await cfn.describeStacks({ StackName: stackName }).promise();
    return result.Stacks![0].Outputs!;
  }

  static getEnv(variableName: string, defaultValue?: string) {
    const variable = process.env[variableName];
    if (!variable) {
      if (defaultValue !== undefined) {
        return defaultValue;
      }
      throw new Error(`${variableName} environment variable must be defined`);
    }
    return variable;
  }

  static addCorsOptions(
    apiResource: apigateway.IResource,
    origin: string,
    allowCredentials: boolean = false,
    allowMethods: string = "OPTIONS,GET,PUT,POST,DELETE"
  ) {
    apiResource.addMethod(
      "OPTIONS",
      new apigateway.MockIntegration({
        integrationResponses: [
          {
            statusCode: "200",
            responseParameters: {
              "method.response.header.Access-Control-Allow-Headers":
                "'Content-Type,X-Amz-Date,Authorization,X-Api-Key,X-Amz-Security-Token,X-Amz-User-Agent'",
              "method.response.header.Access-Control-Allow-Origin":
                "'" + origin + "'",
              "method.response.header.Access-Control-Allow-Credentials":
                "'" + allowCredentials.toString() + "'",
              "method.response.header.Access-Control-Allow-Methods":
                "'" + allowMethods + "'",
              "method.response.header.Access-Control-Max-Age": "'7200'",
            },
          },
        ],
        passthroughBehavior: apigateway.PassthroughBehavior.NEVER,
        requestTemplates: {
          "application/json": '{"statusCode": 200}',
        },
      }),
      {
        methodResponses: [
          {
            statusCode: "200",
            responseParameters: {
              "method.response.header.Access-Control-Allow-Headers": true,
              "method.response.header.Access-Control-Allow-Methods": true,
              "method.response.header.Access-Control-Allow-Credentials": true,
              "method.response.header.Access-Control-Allow-Origin": true,
              "method.response.header.Access-Control-Max-Age": true,
            },
          },
        ],
      }
    );
  }

  static isURL(identityProviderMetadataURLOrFile: string) {
    try {
      new URL(identityProviderMetadataURLOrFile);
      return true;
    } catch {
      return false;
    }
  }

  static isEnvDev(env: string): boolean {
    if (!(env == "Dev" || env == "Prod"))
      throw new Error("Invalid environment Name : " + env);
    return env == "Dev";
  }
}
