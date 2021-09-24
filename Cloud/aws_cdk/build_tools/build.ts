import * as shell from "shelljs";
import { set_env } from "./env";

shell.set("-e");
set_env();

shell.echo("Building IoT App");
shell.exec("npm run build --prefix ../lambda/iot");

// shell.echo("Building CDK");
// shell.exec("npm run build --prefix cdk");

shell.echo("Build successful");
