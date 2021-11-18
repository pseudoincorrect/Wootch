import * as shell from "shelljs";

shell.echo("this will run npm install in all relevant sub-folders");

// npm --prefix has a bug at the time, hence the "cd" commands
shell.cd("..");
const wootch_folder = shell.pwd();

shell.echo("Moving to IOT LAMBDA FUNCTION folder");
shell.cd(`${wootch_folder}`);
shell.cd("lambda");
shell.cd("iot");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing IOT LAMBDA FUNCTION");
shell.exec("npm  install");

shell.echo("Moving to EXPRESS LAMBDA FUNCTION folder");
shell.cd(`${wootch_folder}`);
shell.cd("lambda");
shell.cd("express");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing EXPRESS LAMBDA FUNCTION");
shell.exec("npm  install");

shell.echo("Moving to CDK folder");
shell.cd(`${wootch_folder}`);
shell.cd("cdk");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing CDK");
shell.exec("npm  install");

shell.echo("Moving to ADMIN COMMAND LINE INTERFACE folder");
shell.cd(`${wootch_folder}`);
shell.cd("cli");
shell.cd("admin");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing ADMIN COMMAND LINE INTERFACE");
shell.exec("npm  install");

shell.echo("Moving to USER COMMAND LINE INTERFACE folder");
shell.cd(`${wootch_folder}`);
shell.cd("cli");
shell.cd("user");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing USER COMMAND LINE INTERFACE");
shell.exec("npm  install");

shell.echo("Moving to DEVICE SIMULATOR folder");
shell.cd(`${wootch_folder}`);
shell.cd("device_simulator");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("installing DEVICE SIMULATOR");
shell.exec("npm  install");

shell.echo("Installation successful");
