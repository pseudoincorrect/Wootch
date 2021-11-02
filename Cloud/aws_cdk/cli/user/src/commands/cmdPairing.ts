import * as pairingReq from "../requests/pairingRequest";
import { Args } from "./cmdTypes";

export async function cmdPairing(argv: Args) {
  const pairSecret = argv.secret;
  try {
    const msg = await pairingReq.pairingRequest(pairSecret);
    console.log(msg);
  } catch (error) {
    console.log("Pairing Failed");
    console.log("Try again on your device to generate a pairing secret");
  }
}
