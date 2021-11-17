import * as ses from "@aws-sdk/client-ses";

import * as utils from "../utils/utils";

const awsRegion = utils.getEnv("MAIN_AWS_REGION");
const senderAdress = utils.getEnv("SENDER_ADDRESS");

const sesClient = new ses.SESClient({ region: awsRegion });

/**
 * Send a formated email
 * @param emailAddress email recipient
 */
export async function sendEmail(emailAddress: string) {
  const dateNow = new Date().toISOString();
  const params: ses.SendEmailCommandInput = {
    Destination: {
      CcAddresses: [],
      ToAddresses: [emailAddress],
    },
    Message: {
      Body: {
        Text: {
          Charset: "UTF-8",
          Data: "Your device is emitting an alert ! ",
        },
      },
      Subject: {
        Charset: "UTF-8",
        Data: `Wootch Alert Activity ${dateNow}`,
      },
    },
    Source: senderAdress,
    ReplyToAddresses: [],
  };
  try {
    const data = await sesClient.send(new ses.SendEmailCommand(params));
    console.log("sendEmail Success");
  } catch (err) {
    console.log("sendEmail Error", err);
  }
}
