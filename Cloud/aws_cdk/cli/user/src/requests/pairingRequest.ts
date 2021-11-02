import fetch from "node-fetch";
import { StatusCodes } from "http-status-codes";

import * as secrets from "../../secrets/secrets";
import * as utils from "../utils/filesManipulation";

const host = secrets.HOST_API;

export async function pairingRequest(pairSecret: string): Promise<string> {
  const body = { secret: pairSecret };
  const token = utils.getCredentials();

  const response = await fetch(host + "/pairing/attempt", {
    method: "POST",
    headers: {
      "content-type": "application/json",
      Authorization: token,
    },
    body: JSON.stringify(body),
  });
  const data = await response.json();

  if (response.status == StatusCodes.OK) {
    return data.message;
  } else {
    console.log(data.message);
    throw new Error("Invalid Pairing Request");
  }
}
