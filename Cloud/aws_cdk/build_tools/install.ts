import * as shell from "shelljs";

shell.echo("this will run npm install in all relevant sub-folders");

shell.echo("Installing IoT App");
shell.exec("npm install --prefix ../lambda/iot");

shell.echo("Installing Express App");
shell.exec("npm install --prefix ../lambda/express");

shell.echo("Installing CDK");
shell.exec("npm install --prefix ../cdk");

shell.echo("Installing Admin Command Line Interface");
shell.exec("npm install --prefix ../cli/admin");

shell.echo("Installation successful");
