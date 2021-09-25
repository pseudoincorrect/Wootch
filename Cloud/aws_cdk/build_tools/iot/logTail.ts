import * as shell from "shelljs";

shell.echo("Listening to WootchDevTopicLambda Logs");

// shell.exec(
//   "awslogs get /aws/lambda/WootchDevTopicLambda ALL --watch --start='2m'"
// );

const logTailCmd =
  "aws logs tail /aws/lambda/WootchDevTopicLambda --follow --format short";
shell.echo(logTailCmd);
shell.exec(logTailCmd);
