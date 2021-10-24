import * as shell from "shelljs";

shell.echo("Listening to WootchDevTopicLambda Logs");

const logTailCmd =
  "aws logs tail /aws/lambda/WootchDevTopicLambda --follow --format short";
shell.echo(logTailCmd);
shell.exec(logTailCmd);
