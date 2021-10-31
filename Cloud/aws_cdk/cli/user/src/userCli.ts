import yargs from "yargs";
import * as usr from "./commands/cmdUser";

const cliName = "userCli";

function adminCli() {
  yargs
    // Login
    .command(
      "login",
      "Login to access wootch user services",
      (yargs: any) => {
        yargs
          .option("email", {
            alias: "e",
            description: "email of the user",
            type: "string",
            required: true,
          })
          .option("password", {
            alias: "p",
            description: "password of the user",
            type: "string",
            required: true,
          })
          .help();
      },
      usr.cmdLogin
    )
    // Cli Options
    .version()
    .alias("v", "version")
    .help()
    .alias("h", "help")
    .showHelpOnFail(true)

    .example(
      `${cliName} login --email jean-george@chezlui.fr --password Password123?`,
      `Login and receive your credentials`
    )
    .parse();
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  adminCli();
}
