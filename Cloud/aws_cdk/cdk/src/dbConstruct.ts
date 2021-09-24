
import * as cdk from "@aws-cdk/core";
import * as dynamodb from "@aws-cdk/aws-dynamodb";
import * as timeStream from "@aws-cdk/aws-timestream";

import { CdkContext, Utils } from "./utils";
import { CfnTable } from "@aws-cdk/aws-dynamodb";
import { RemovalPolicy } from "@aws-cdk/core";

export interface StackDatabases {
  sensorDataDb: timeStream.CfnDatabase;
  sensorDataTable: timeStream.CfnTable;
  mainTable: dynamodb.Table;
}

export interface DbConstructProps {
  cdkContext: CdkContext;
}

export class DbConstruct extends cdk.Construct {
  public dbs: StackDatabases;

  constructor(scope: cdk.Construct, id: string, props: DbConstructProps) {
    super(scope, id);

    const env = props.cdkContext.env;
    const stackAndEnv = props.cdkContext.stackName + env;

    //-------------------------------------------------------------------------
    // TimeStream
    //-------------------------------------------------------------------------

    const sensorDataDbId = stackAndEnv + "SensorDataDb";
    const sensorDataDb = new timeStream.CfnDatabase(this, sensorDataDbId, {
      databaseName: sensorDataDbId,
    });

    const sensorDataTableId = stackAndEnv + "SensorDataTable";
    const sensorDataTable = new timeStream.CfnTable(this, sensorDataTableId, {
      databaseName: sensorDataDbId,
      tableName: sensorDataTableId,
      retentionProperties: {
        memoryStoreRetentionPeriodInHours: (24).toString(10),
        magneticStoreRetentionPeriodInDays: (365 * 2).toString(10),
      },
    });

    sensorDataTable.node.addDependency(sensorDataDb);

    // keep the table when stack get deleted (cdk destroy)
    if (Utils.isEnvDev(env))
      sensorDataTable.applyRemovalPolicy(cdk.RemovalPolicy.DESTROY);
    else sensorDataTable.applyRemovalPolicy(cdk.RemovalPolicy.RETAIN);

    //-------------------------------------------------------------------------
    // Dynamodb
    //------------------------------------------------------------------------

    const mainTableId = `${stackAndEnv}AppTable`;

    const mainTable = new dynamodb.Table(this, mainTableId, {
      tableName: mainTableId,
      billingMode: dynamodb.BillingMode.PAY_PER_REQUEST,
      serverSideEncryption: true,
      stream: dynamodb.StreamViewType.NEW_AND_OLD_IMAGES,
      partitionKey: {
        name: "PK",
        type: dynamodb.AttributeType.STRING,
      },
      sortKey: {
        name: "SK",
        type: dynamodb.AttributeType.STRING,
      },
      timeToLiveAttribute: "ttl",
    });

    mainTable.addGlobalSecondaryIndex({
      indexName: "SK_SORT_KEY",
      partitionKey: {
        name: "SK",
        type: dynamodb.AttributeType.STRING,
      },
      projectionType: dynamodb.ProjectionType.ALL,
    });

    const mainTableCfn = mainTable.node.defaultChild as CfnTable;
    if (Utils.isEnvDev(env))
      mainTableCfn.applyRemovalPolicy(RemovalPolicy.DESTROY);
    else mainTableCfn.applyRemovalPolicy(RemovalPolicy.RETAIN);

    this.dbs = {
      mainTable,
      sensorDataDb,
      sensorDataTable,
    };
  }
}
