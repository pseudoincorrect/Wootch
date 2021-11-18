import * as shell from "shelljs";

shell.echo(
  "this programm will build (transpilation and packing) all relevant sub-projects"
);

// npm --prefix has a bug at the time, hence the "cd" commands
shell.cd("..");
shell.cd("lambda");
const lambda_folder = shell.pwd();

shell.echo("Moving to IOT LAMBDA FUNCTION folder");
shell.cd(`${lambda_folder}`);
shell.cd("iot");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("Building IOT LAMBDA FUNCTION (typescript, zip)");
shell.exec("npm run typescriptBuild");
shell.exec("npm run zipDist");

shell.echo("Moving to EXPRESS LAMBDA FUNCTION folder");
shell.cd(`${lambda_folder}`);
shell.cd("express");
shell.echo("In folder: ", shell.pwd().toString());
shell.echo("Building EXPRESS LAMBDA FUNCTION (typescript, webpack, zip)");
shell.exec("npm run typescriptBuildWithModules");
shell.exec("npm run webpack");
shell.exec("npm run zipPack");

shell.echo("Build successful");
