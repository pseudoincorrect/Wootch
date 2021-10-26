#!/usr/bin/env node
import * as cdk from "@aws-cdk/core";

import { CognitoConstruct } from "./cognitoConstruct";
import { DbConstruct } from "./dbConstruct";
import { IotConstruct } from "./iotConstruct";
import { CdkContext, Utils } from "./utils";

// import { ExpressConstruct } from "./expressConstruct";
interface WootchStackProps extends cdk.StackProps {
  cdkContext: CdkContext;
}

export class WootchStack extends cdk.Stack {
  constructor(scope: cdk.Construct, id: string, props: WootchStackProps) {
    super(scope, id, props);

    const stackDbs = new DbConstruct(this, "Databases", {
      cdkContext: props.cdkContext,
    });

    const cognitoConstruct = new CognitoConstruct(this, "Cognito", {
      cdkContext: props.cdkContext,
    });

    // new ExpressConstruct(this, "Express", {
    //   cdkContext: props.cdkContext,
    //   userPool: cognitoConstruct.userPool,
    //   stackDbs: stackDbs.dbs,
    // });

    new IotConstruct(this, "Iot", {
      cdkContext: props.cdkContext,
      stackDbs: stackDbs.dbs,
    });

    new cdk.CfnOutput(this, "Region", {
      description: "Region",
      value: this.region,
    });

    new cdk.CfnOutput(this, "RetainPolicy", {
      description: "CdkDestroyRetainPolicy",
      value: Utils.isEnvDev(props.cdkContext.env)
        ? "DESTROY_EVERYTHING"
        : "RETAIN",
    });
  }
}

const app = new cdk.App();
const cdkContext = Utils.getCdkContext(app);
const stackAndEnv = cdkContext.stackName + cdkContext.env;

new WootchStack(app, stackAndEnv, { cdkContext });
