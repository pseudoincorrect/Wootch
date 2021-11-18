import * as shell from "shelljs";

shell.echo("Listening to IOT lambda function Logs");

// Output the log (cloudwatch) of the iot function, continuously
const logTailCmd =
  "aws logs tail /aws/lambda/WootchDevIotFunction --follow --format short";

const logTailCmdWithFilter =
  'aws logs tail /aws/lambda/WootchDevIotFunction --output text --follow --format short --filter-pattern  "?INFO ?ERROR"';

shell.echo(logTailCmd);
shell.exec(logTailCmd);
