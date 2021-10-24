import * as shell from "shelljs";

shell.echo(
  "this programm will build (transpilation and packing) all relevant sub-projects"
);

shell.echo("Building IoT App (typescript)");
shell.exec("npm run build --prefix ../lambda/iot");

shell.echo("Building IoT App (typescript, webpack, zip)");
shell.exec("npm run build --prefix ../lambda/express");
shell.exec("npm run pack --prefix ../lambda/express");
shell.exec("npm run zipPack --prefix ../lambda/express");

shell.echo("Build successful");
