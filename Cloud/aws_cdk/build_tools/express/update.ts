import * as shell from "shelljs";
import * as cst from "../constants";

// All paths are relative to "Express's package.json", not to this file's location

// Build the function with typescript and copy the node module in dist/ folder
shell.exec("npm run typescriptBuildWithModules --prefix ../lambda/express");

// User webpack to compress javascript data
shell.exec("npm run webpack --prefix ../lambda/express");

// Zip all the data
shell.exec("npm run zipPack --prefix ../lambda/express");

// Update the associated lambda funciton with the ziped data
const updtFuncCmd = `aws lambda update-function-code \
    --region ${cst.stackRegion} \
    --function-name ${cst.apiFunctionName} \
    --zip-file fileb://../lambda/express/dist/function.zip`;
shell.exec(updtFuncCmd);

// Delete the zipped data
shell.rm("-rf", "../lambda/express/dist/function.zip");
