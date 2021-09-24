import * as shell from "shelljs";
import { set_env } from "../env";

shell.set("-e");
set_env();

const stackName = shell.env["STACK_NAME"];
const stackRegion = shell.env["STACK_REGION"];

const t1 = JSON.parse(
  shell
    .exec(`aws cloudformation describe-stacks --stack-name ${stackName}`, {
      silent: true,
    })
    .toString()
);

const functOutputKey = t1["Stacks"][0]["Outputs"].find(
  (i: any) => i["Description"] == "IoT topic lambda function name"
)["OutputValue"];

// All path are relative to "package.json", not to this file's location
shell.exec("npm run build --prefix ../lambda/iot");
shell.exec("npm run package --prefix ../lambda/iot");

const updtFuncCmd = `aws lambda update-function-code \
    --region ${stackRegion} \
    --function-name ${functOutputKey} \
    --zip-file fileb://../lambda/iot/dist/function.zip`;

shell.exec(updtFuncCmd);

shell.rm("-rf", "../lambda/iot/dist/function.zip");
