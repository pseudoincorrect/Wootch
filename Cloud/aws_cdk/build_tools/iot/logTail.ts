import * as shell from "shelljs";

shell.echo("Listening to StayqoolDevTopicLambda Logs");

// shell.exec(
//   "awslogs get /aws/lambda/StayqoolDevTopicLambda ALL --watch --start='2m'"
// );

const logTailCmd =
  "aws logs tail /aws/lambda/StayqoolDevTopicLambda --follow --format short";
shell.echo(logTailCmd);
shell.exec(logTailCmd);
