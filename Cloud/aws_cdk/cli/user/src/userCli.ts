import yargs from "yargs";

import * as login from "./commands/cmdLogin";
import * as pairing from "./commands/cmdPairing";

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
      login.cmdLogin
    )
    // Pairing
    .command(
      "pairing",
      "Pairing a device with a user through a common secret",
      (yargs: any) => {
        yargs
          .option("secret", {
            alias: "s",
            description: "pairing secret given by the device",
            type: "string",
            required: true,
          })
          .help();
      },
      pairing.cmdPairing
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
    .example(
      `${cliName} pairing --secret 1A2B3C`,
      `Pair with the device that issued the secret "1A2B3C"`
    )
    .parse();
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  adminCli();
}
