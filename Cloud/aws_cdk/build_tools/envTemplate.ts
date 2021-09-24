import * as shell from "shelljs";

// ======================================================================
// TO FILL : AWS_SDK_LOAD_CONFIG (eg: "1")
// allows the SDK to load from config. see https://github.com/aws/aws-sdk-js/pull/1391
// ======================================================================

const aws_sdk_load_config = "";

// ======================================================================
// TO FILL : STACK_NAME
// the CloudFormation stack name, e.g. "MyAppName"
// ======================================================================

const stack_name = "";

// ===========================================================================
// Other Env Variable gotten from aws-cli (no input needed)
// ===========================================================================

export function set_env() {
  shell.env["AWS_SDK_LOAD_CONFIG"] = aws_sdk_load_config;

  shell.env["STACK_NAME"] = stack_name;

  // explicitly define the account you intend to deploy into
  // for the simplicity of running the demo, it takes the current profile's account and region
  // in production make sure you explicitly define these via the CI/CD environment variables as a safety mechanism
  const stack_account = shell
    .exec('aws sts get-caller-identity --query "Account" --output text')
    .toString()
    .trim();
  shell.env["STACK_ACCOUNT"] = stack_account;

  const stack_region = shell.exec("aws configure get region").toString().trim();
  shell.env["STACK_REGION"] = stack_region;

  // change to any unused domain name, default to a combination that is be unique per account+region
  shell.env["STACK_ACCOUNT"] = `auth-${stack_account}-${stack_region}`;
  console.log(`auth-${stack_account}-${stack_region}`);

  // local mode: use this for development (the UI is not deployed and served locally)
  shell.env["APP_URL"] = "http://localhost:3000";
}
