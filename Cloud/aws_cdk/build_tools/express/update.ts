import * as shell from "shelljs";
import * as cst from "../constants";

// All path are relative to "package.json", not to this file's location
shell.exec("npm run build --prefix ../lambda/express");
shell.exec("npm run pack --prefix ../lambda/express");
shell.exec("npm run zipPack --prefix ../lambda/express");

const updtFuncCmd = `aws lambda update-function-code \
    --region ${cst.stackRegion} \
    --function-name ${cst.apiFunctionName} \
    --zip-file fileb://../lambda/express/dist/function.zip`;

shell.exec(updtFuncCmd);

shell.rm("-rf", "../lambda/express/dist/function.zip");
