import yargs from "yargs";
import * as dev from "./commands/device";

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
      dev.createDeviceCmd
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
      dev.searchDeviceCmd
    )
    // Create Device Type Command
    .command(
      "createDeviceType",
      "Create a device type (mandatory to create a device)",
      dev.createDeviceTypeCmd
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
      dev.deleteDeviceCmd
    )
    // Cli Options
    .version()
    .alias("v", "version")
    .help()
    .alias("h", "help")
    .showHelpOnFail(true)
    .example(`$0 createDevice --id 123456`, `Create a device with ID 123456`)
    .example(`$0 createDeviceType`, `Register a unique device type on AWS`)
    .example(`$0 deleteDevice --id 123456`, `Delete the device with ID 123456`)
    .parse();
}

// ===========================================================================
// Main
// ===========================================================================
if (require.main === module) {
  adminCli();
}
