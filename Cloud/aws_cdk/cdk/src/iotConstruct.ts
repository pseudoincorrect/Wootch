import * as cdk from "@aws-cdk/core";
import * as lambda from "@aws-cdk/aws-lambda";
import * as iot from "@aws-cdk/aws-iot";
import * as iam from "@aws-cdk/aws-iam";
import * as iToL from "@aws-solutions-constructs/aws-iot-lambda";
import { CdkContext } from "./utils";
import { StackDatabases } from "./dbConstruct";

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
    const region = props.cdkContext.region;
    const accountId = props.cdkContext.accountId;
    const topic = stackAndEnv + "/device/+/data/#";

    //-------------------------------------------------------------------------
    // Lambda Function
    //-------------------------------------------------------------------------

    const IotFunctionId = stackAndEnv + "IotFunction";

    const IotFunction: iToL.IotToLambdaProps = {
      lambdaFunctionProps: {
        functionName: IotFunctionId,
        runtime: lambda.Runtime.NODEJS_10_X,
        code: lambda.Code.fromAsset("../lambda/iot/dist/src"),
        handler: "iotReceive.handler",
        environment: {
          SENSOR_DATA_DB: props.stackDbs.sensorDataDb.databaseName!,
          SENSOR_DATA_TABLE: props.stackDbs.sensorDataTable.tableName!,
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
      IotFunction
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
        IotFunction.lambdaFunctionProps?.functionName || "IotFunction error",
    });
  }
}
