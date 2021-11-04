import * as shell from "shelljs";

shell.echo("Listening to WootchDevTopicLambda Logs");

// Output the log (cloudwatch) of the iot function, continuously
const logTailCmd =
  "aws logs tail /aws/lambda/WootchDevTopicLambda --follow --format short";
shell.echo(logTailCmd);
shell.exec(logTailCmd);
