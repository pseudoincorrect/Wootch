import * as shell from "shelljs";
import * as cst from "../constants";

// All path are relative to "Iot's package.json", not to this file's location

// Build the function with typescript and copy the node module in dist/ folder
shell.exec("npm run typescriptBuild --prefix ../lambda/iot");

// Zip all the data
shell.exec("npm run zipDist --prefix ../lambda/iot");

// Update the associated lambda function with the ziped data
const updtFuncCmd = `aws lambda update-function-code \
--region ${cst.stackRegion} \
--function-name ${cst.iotFunctionName} \
--zip-file fileb://../lambda/iot/dist/function.zip`;
shell.exec(updtFuncCmd);

// Delete the zipped data
shell.rm("-rf", "../lambda/iot/dist/function.zip");
