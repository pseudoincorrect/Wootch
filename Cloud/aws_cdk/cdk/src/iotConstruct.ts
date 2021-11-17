import * as iam from "@aws-cdk/aws-iam";
import * as lambda from "@aws-cdk/aws-lambda";
import * as cdk from "@aws-cdk/core";
import * as iToL from "@aws-solutions-constructs/aws-iot-lambda";
import * as secrets from "../secrets/secrets";

import { StackDatabases } from "./dbConstruct";
import { CdkContext } from "./utils";

export interface IotConstructProps {
  cdkContext: CdkContext;
  stackDbs: StackDatabases;
}

export class IotConstruct extends cdk.Construct {
  constructor(scope: cdk.Construct, id: string, props: IotConstructProps) {
    super(scope, id);

    //-------------------------------------------------------------------------
    // Construct Constants
    //-------------------------------------------------------------------------

    const stackAndEnv = props.cdkContext.stackName + props.cdkContext.env;
    const region = secrets.WOOTCH_AWS_REGION;
    const topic = stackAndEnv + "/#";
    const receiverAddress = secrets.RECEIVER_ADDRESS;
    const senderAddress = secrets.SENDER_ADDRESS;
    //-------------------------------------------------------------------------
    // Lambda Function
    //-------------------------------------------------------------------------

    const iotFunctionId = stackAndEnv + "IotFunction";

    const iotFunction: iToL.IotToLambdaProps = {
      lambdaFunctionProps: {
        functionName: iotFunctionId,
        runtime: lambda.Runtime.NODEJS_14_X,
        code: lambda.Code.fromAsset("../lambda/iot/dist/src"),
        handler: "iotReceive.handler",
        environment: {
          SENSOR_DATA_DB: props.stackDbs.sensorDataDb.databaseName!,
          SENSOR_DATA_TABLE: props.stackDbs.sensorDataTable.tableName!,
          MAIN_DATA_TABLE: props.stackDbs.mainTable.tableName,
          MAIN_AWS_REGION: region,
          SENDER_ADDRESS: senderAddress,
          RECEIVER_ADDRESS: receiverAddress,
        },
      },
      iotTopicRuleProps: {
        topicRulePayload: {
          ruleDisabled: false,
          description: `Process any data from '${topic}' topic`,
          sql: `SELECT *, topic() AS topic, clientid() AS clientid FROM '${topic}'`,
          actions: [],
        },
      },
    };
    const topicIotLambda = new iToL.IotToLambda(
      this,
      stackAndEnv + "TopicIotLambda",
      iotFunction
    );

    // Dynamodb Policy
    props.stackDbs.mainTable.grantReadWriteData(
      topicIotLambda.lambdaFunction.role!
    );

    const iotFunctionPolicies = [
      new iam.PolicyStatement({
        effect: iam.Effect.ALLOW,
        actions: [
          "timestream:ListMeasures",
          "timestream:WriteRecords",
          "timestream:Select",
          "timestream:DescribeTable",
        ],
        resources: [`${props.stackDbs.sensorDataTable.attrArn}`],
      }),
      new iam.PolicyStatement({
        effect: iam.Effect.ALLOW,
        actions: ["timestream:DescribeEndpoints"],
        resources: ["*"],
      }),
    ];

    for (const p of iotFunctionPolicies) {
      topicIotLambda.lambdaFunction.addToRolePolicy(p);
    }

    //-------------------------------------------------------------------------
    // Outputs
    //-------------------------------------------------------------------------

    new cdk.CfnOutput(this, "IotTopicFunctionName", {
      description: "IoT topic lambda function name",
      value:
        iotFunction.lambdaFunctionProps?.functionName || "iotFunction error",
    });
  }
}
