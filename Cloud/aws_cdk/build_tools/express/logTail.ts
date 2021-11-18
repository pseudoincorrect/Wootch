import * as shell from "shelljs";

shell.echo("Listening to EXPRESS lambda function Logs");

// Output the log (cloudwatch) of the iot function, continuously
const logTailCmd =
  "aws logs tail /aws/lambda//aws/lambda/WootchDevAPIFunction --follow --format short";

const logTailCmdWithFilter =
  'aws logs tail /aws/lambda//aws/lambda/WootchDevAPIFunction --output text --follow --format short --filter-pattern  "?INFO ?ERROR"';

shell.echo(logTailCmd);
shell.exec(logTailCmd);
