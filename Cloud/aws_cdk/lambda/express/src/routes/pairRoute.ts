import { NextFunction, Request, Response, Router } from "express";
import { body, validationResult } from "express-validator";
import { StatusCodes } from "http-status-codes";

import { minAuth } from "../middlewares/authorizationMiddleware";
import { DevModel } from "../models/devModel";
import { PairModel } from "../models/pairingModel";
import { UserModel } from "../models/userModel";
import { DevDdb } from "../storage/devDdb";
import { PairingDdb } from "../storage/pairingDdb";
import { UserDdb } from "../storage/userDdb";
import { AppValidationError } from "../utils/appErrors";
import { authGroups } from "../utils/authGroups";
import { publishUser } from "../iot/pairingIot";

const router = Router();
const pairingDdb = new PairingDdb();
const devDdb = new DevDdb();
const userDdb = new UserDdb();

/******************************************************************************
 * POST Pairing attempt - "POST /pairing/attempt"
 * Attempt to link a device to a user through a shared secret (pairing key)
 ******************************************************************************/
router.post(
  "/attempt",
  [minAuth(authGroups.users)],
  body("secret").isAlphanumeric().isLength({ min: 6, max: 6 }),
  async (req: Request, res: Response, next: NextFunction) => {
    if (!validationResult(req).isEmpty()) {
      const msg = validationResult(req).array()[0].param;
      next(new AppValidationError(`POST user invalid params : ${msg}`));
      return;
    }

    // Check the pairing key exists
    let pairModel: PairModel | undefined;
    const pairId = req.body.secret;
    try {
      pairModel = await pairingDdb.getPairing(pairId);
      console.log(pairModel);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "not a valid pairing request secret" });
    }

    // Get the device model associated with the pairing key
    const devKey = pairModel!.DEV_KEY;
    console.log(`devKey ${devKey}`);
    let devModel: DevModel | undefined;
    try {
      devModel = await devDdb.getDev(devKey);
      console.log(devModel);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "Device does not exist" });
    }

    // Get the user model from credential
    const devId = devModel?.thingName();
    const userId = req.userId;
    const userKey = UserModel.getPkFromId(userId);
    console.log(`userKey ${userKey}`);
    let userModel: UserModel | undefined;
    try {
      userModel = await userDdb.getUser(userKey);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "User does not exist" });
    }

    // Associate the user to the device
    const userEmail = userModel!.USER_EMAIL;
    devModel!.DEV_USER_KEY = userKey;
    try {
      await devDdb.updateDevUserKey(devModel!);
    } catch (error) {
      console.log(error);
      return res.status(StatusCodes.NOT_FOUND).json({
        message: `Could not pair user ${userEmail} to device ${devKey}`,
      });
    }

    try {
      await publishUser(devId!, userEmail);
    } catch (error) {
      console.log(error);
      return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
        message: `could not publish user mail to the device`,
      });
    }

    // delete the pairing request
    try {
      await pairingDdb.deletePairing(pairId);
    } catch (error) {
      console.log(error);
      return res.status(StatusCodes.NOT_FOUND).json({
        message: `Could not delete pair ${pairId}`,
      });
    }

    return res.status(StatusCodes.OK).json({
      message: `Paired user ${userEmail} to device ${devKey}`,
    });
  }
);

export default router;
