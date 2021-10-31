import * as shell from "shelljs";
import * as cst from "../constants";

// All path are relative to "package.json", not to this file's location
shell.exec("npm run build --prefix ../lambda/iot");
shell.exec("npm run package --prefix ../lambda/iot");

const updtFuncCmd = `aws lambda update-function-code \
    --region ${cst.stackRegion} \
    --function-name ${cst.iotFunctionName} \
    --zip-file fileb://../lambda/iot/dist/function.zip`;

shell.exec(updtFuncCmd);

shell.rm("-rf", "../lambda/iot/dist/function.zip");
