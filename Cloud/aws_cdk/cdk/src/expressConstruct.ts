import * as apigw from "@aws-cdk/aws-apigateway";
import * as cert from "@aws-cdk/aws-certificatemanager";
import * as cognito from "@aws-cdk/aws-cognito";
import * as iam from "@aws-cdk/aws-iam";
import * as lambda from "@aws-cdk/aws-lambda";
import * as r53 from "@aws-cdk/aws-route53";
import * as r53Rt from "@aws-cdk/aws-route53-targets";
import * as cdk from "@aws-cdk/core";

import * as cst from "./constants";
import * as secrets from "../secrets/secrets";
import { StackDatabases } from "./dbConstruct";
import { CdkContext } from "./utils";

export interface ExpressConstructProps {
  cdkContext: CdkContext;
  stackDbs: StackDatabases;
  userPool: cognito.UserPool;
}

export class ExpressConstruct extends cdk.Construct {
  constructor(scope: cdk.Construct, id: string, props: ExpressConstructProps) {
    super(scope, id);

    const region = secrets.WOOTCH_AWS_REGION;

    const env = props.cdkContext.env;
    const stackAndEnv = props.cdkContext.stackName + env;
    const useCustomDomain = props.cdkContext.useCustomDomain;
    const domainHostedZone = props.cdkContext.domainHostedZone;
    const certificateArn = props.cdkContext.certificateArn;
    const domainName = props.cdkContext.domainName;

    const nodeRuntime: lambda.Runtime = lambda.Runtime.NODEJS_14_X;
    const lambdaMemory: number = 128;

    ///////////////////////////////////////////////////////////////////////////////

    const apiFunction = new lambda.Function(this, "APIFunction", {
      functionName: stackAndEnv + "APIFunction",
      runtime: nodeRuntime,
      handler: "index.handler",
      code: lambda.Code.fromAsset("../lambda/express/dist/packed"),
      timeout: cdk.Duration.seconds(10),
      memorySize: lambdaMemory,
      environment: {
        AWS_APP_REGION: region,
        APP_TABLE_NAME: props.stackDbs.mainTable.tableName!,
        SENSOR_DATA_DB_NAME: props.stackDbs.sensorDataDb.databaseName!,
        SENSOR_DATA_TABLE_NAME: props.stackDbs.sensorDataTable.tableName!,
        ADMINS_GROUP_NAME: cst.adminsGroupName,
        USERS_GROUP_NAME: cst.usersGroupName,
        USER_POOL_ID: props.userPool.userPoolId,
        AUTHORIZATION_HEADER_NAME: cst.authorizationHeaderName,
      },
    });

    ///////////////////////////////////////////////////////////////////////////////

    // Dynamodb Policy
    props.stackDbs.mainTable.grantReadWriteData(apiFunction.role!);

    const apiPolicies = [
      // Timestream Policy
      new iam.PolicyStatement({
        effect: iam.Effect.ALLOW,
        actions: [
          "timestream:Select",
          "timestream:ListMeasures",
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

    for (const p of apiPolicies) {
      apiFunction.addToRolePolicy(p);
    }

    ///////////////////////////////////////////////////////////////////////////////

    //API Gateway
    const api = new apigw.RestApi(this, "APIGateway", {
      restApiName: stackAndEnv + "APIGateway",
      deployOptions: { stageName: stackAndEnv },
      endpointTypes: [apigw.EndpointType.EDGE],
    });

    const integration = new apigw.LambdaIntegration(apiFunction, {
      proxy: true,
    });

    const cfnAuthorizer = new apigw.CfnAuthorizer(this, "APIauth", {
      name: "CognitoAuthorizer",
      type: apigw.AuthorizationType.COGNITO,
      identitySource: "method.request.header." + cst.authorizationHeaderName,
      restApiId: api.restApiId,
      providerArns: [props.userPool.userPoolArn],
    });

    const rootResource = api.root;
    rootResource.addMethod("ANY", integration);
    const proxyResource = rootResource.addResource("{proxy+}");
    proxyResource.addMethod("ANY", integration, {
      authorizer: { authorizerId: cfnAuthorizer.ref },
      authorizationType: apigw.AuthorizationType.COGNITO,
    });

    ///////////////////////////////////////////////////////////////////////////////
    // Route53

    // Note: this process is not entirely automated with CDK.
    // You need to register a domain name with route53 and need to create
    // a ACM certificate with AWS Certificate Manager
    // The certificate need to certify "example.com" and "api.example.com"
    // Then use the certificate ARN and domain name for bellow
    //
    // https://medium.com/@gregorypierce/cdk-restapi-custom-domains-554175a4b1f6

    // We will use this part when a new deployment happen (with new certificate)
    if (useCustomDomain) {
      const hostedZone = r53.HostedZone.fromHostedZoneId(
        this,
        "hostedZone",
        domainHostedZone
      );

      const certificate = cert.Certificate.fromCertificateArn(
        this,
        "certificate",
        certificateArn
      );

      // Map the api to teh domain name
      api.addDomainName("ApiDomainName", {
        domainName: domainName,
        securityPolicy: apigw.SecurityPolicy.TLS_1_2,
        certificate: certificate,
      });

      // Map the api to the sub-domain name
      // seems very similart to above, but necessary
      new apigw.DomainName(this, "api_domain_name", {
        domainName: "api" + "." + domainName,
        securityPolicy: apigw.SecurityPolicy.TLS_1_2,
        certificate: certificate,
        endpointType: apigw.EndpointType.EDGE,
        mapping: api,
      });

      // setup the alias record that will point to this API
      new r53.ARecord(this, "domain_alias_record", {
        recordName: "api" + "." + domainName,
        zone: hostedZone,
        target: r53.RecordTarget.fromAlias(new r53Rt.ApiGateway(api)),
      });
    }

    ///////////////////////////////////////////////////////////////////////////////

    // Publish the custom resource output
    new cdk.CfnOutput(this, "APIUrlOutput", {
      description: "API URL",
      value: api.url ? api.url : "no URL",
    });

    new cdk.CfnOutput(this, "ApiFunctionName", {
      description: "API lambda function name",
      value: apiFunction.functionName,
    });

    ///////////////////////////////////////////////////////////////////////////////
  }
}
