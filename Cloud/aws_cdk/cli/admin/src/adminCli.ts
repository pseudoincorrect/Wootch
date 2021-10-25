import yargs from "yargs";

import * as dev from "./commands/cmdDevice";
import * as usr from "./commands/cmdUser";

const cliName = "adminCli";

function adminCli() {
  yargs
    // Create Device Command
    .command(
      "createDevice",
      "Create a device with an ID",
      (yargs: any) => {
        yargs
          .option("id", {
            alias: "i",
            description: "ID of the device",
            type: "string",
            required: true,
          })
          .help();
      },
      dev.cmdDeviceCreate
    )
    // Delete Device Command
    .command(
      "deleteDevice",
      "Delete a device by ID",
      (yargs: any) => {
        yargs
          .option("id", {
            alias: "i",
            description: "ID of the device",
            type: "string",
            required: true,
          })
          .help();
      },
      dev.cmdDeviceDelete
    )
    // Search Device Command
    .command(
      "searchDevice",
      "Search a device and its info",
      (yargs: any) => {
        yargs
          .option("id", {
            alias: "i",
            description: "ID of the device",
            type: "string",
            required: true,
          })
          .help();
      },
      dev.cmdDeviceSearch
    )
    // Create a device ID
    .command(
      "createDeviceId",
      "Create a unique device ID",
      dev.cmdCreateDeviceId
    )
    // Create an user
    .command(
      "createUser",
      "Create a regular user with temporary password",
      (yargs: any) => {
        yargs
          .option("email", {
            alias: "e",
            description: "Email of the user",
            type: "string",
            required: true,
          })
          .option("password", {
            alias: "p",
            description: "Password of the user",
            type: "string",
            required: true,
          });
      },
      usr.cmdCreateUser
    )
    // Delete an user
    .command(
      "deleteUser",
      "Delete an user",
      (yargs: any) => {
        yargs.option("email", {
          alias: "e",
          description: "Email of the user",
          type: "string",
          required: true,
        });
      },
      usr.cmdDeleteUser
    )
    // Cli Options
    .version()
    .alias("v", "version")
    .help()
    .alias("h", "help")
    .showHelpOnFail(true)

    .example(
      `${cliName} createUser --email jean-george@chezlui.fr --password Password123?`,
      `Create a user with a temporary password`
    )
    .example(
      `${cliName} deleteUser --email jean-george@chezlui.fr`,
      `Delete a user`
    )
    .example(
      `${cliName} createDevice --id 123456`,
      `Create a device with ID 123456`
    )
    .example(
      `${cliName} deleteDevice --id 123456`,
      `Delete the device with ID 123456`
    )
    .example(
      `${cliName} searchDevice --id 123456`,
      `Search for a device with ID 123456`
    )
    .parse();
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  adminCli();
}
