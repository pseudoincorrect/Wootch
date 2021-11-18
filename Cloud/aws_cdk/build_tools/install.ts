import * as shell from "shelljs";

shell.echo("this will run npm install in all relevant sub-folders");

shell.cd("..");
const wootch_folder = shell.pwd();

shell.echo("Installing IoT App");
shell.cd(`${wootch_folder}`);
shell.cd("lambda");
shell.cd("iot");
shell.exec("npm  install");

shell.echo("Installing Express App");
shell.cd(`${wootch_folder}`);
shell.cd("lambda");
shell.cd("express");
shell.exec("npm  install");

shell.echo("Installing CDK");
shell.cd(`${wootch_folder}`);
shell.cd("cdk");
shell.exec("npm  install");

shell.echo("Installing Admin Command Line Interface");
shell.cd(`${wootch_folder}`);
shell.cd("cli");
shell.cd("admin");
shell.exec("npm  install");

shell.echo("Installing User Command Line Interface");
shell.cd(`${wootch_folder}`);
shell.cd("cli");
shell.cd("user");
shell.exec("npm  install");

shell.echo("Installing Device Simulator");
shell.cd(`${wootch_folder}`);
shell.cd("device_simulator");
shell.exec("npm  install");

shell.echo("Installation successful");
