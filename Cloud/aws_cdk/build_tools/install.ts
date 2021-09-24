import * as shell from "shelljs";
import { set_env } from "./env";

shell.set("-e");
set_env();

shell.echo("this will run npm install in all relevant sub-folders");

shell.echo("Installing IoT App");
shell.exec("npm install --prefix ../lambda/iot");

// shell.echo("Installing CDK");
// shell.exec("npm install --prefix ../cdk");

shell.echo("Installation successful");
