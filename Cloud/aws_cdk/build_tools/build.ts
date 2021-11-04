import * as shell from "shelljs";

shell.echo(
  "this programm will build (transpilation and packing) all relevant sub-projects"
);

shell.echo("Building IoT App (typescript)");
shell.exec("npm run typescriptBuild --prefix ../lambda/iot");
shell.exec("npm run zipDist --prefix ../lambda/iot");

shell.echo("Building Express App (typescript, webpack, zip)");
shell.exec("npm run typescriptBuildWithModules --prefix ../lambda/express");
shell.exec("npm run webpack --prefix ../lambda/express");
shell.exec("npm run zipPack --prefix ../lambda/express");

shell.echo("Build successful");
