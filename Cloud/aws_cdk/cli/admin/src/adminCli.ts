import yargs from "yargs";
import * as dev from "./commands/cmdDevice";

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
    // Cli Options
    .version()
    .alias("v", "version")
    .help()
    .alias("h", "help")
    .showHelpOnFail(true)
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
