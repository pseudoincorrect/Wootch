import { NextFunction, Request, Response, Router } from "express";
import { body, validationResult } from "express-validator";
import { StatusCodes } from "http-status-codes";

import { minAuth } from "../middlewares/authorizationMiddleware";
import { DevModel } from "../models/devModel";
import { PairModel } from "../models/pairingModel";
import { UserModel } from "../models/userModel";
import { DevAccess } from "../storage/devAccess";
import { PairingAccess } from "../storage/pairingAccess";
import { UserAccess } from "../storage/userAccess";
import { AppValidationError } from "../utils/appErrors";
import { authGroups } from "../utils/authGroups";

const router = Router();
const pairingAccess = new PairingAccess();
const devAccess = new DevAccess();
const userAccess = new UserAccess();

/******************************************************************************
 *  POST Pairing attempt - "POST /pairing/attempt"
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

    let pairModel: PairModel | undefined;
    const pairId = req.body.secret;
    try {
      pairModel = await pairingAccess.getPairing(pairId);
      console.log(pairModel);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "not a valid pairing request secret" });
    }

    const devKey = pairModel!.DEV_KEY;
    console.log(`devKey ${devKey}`);
    let devModel: DevModel | undefined;
    try {
      devModel = await devAccess.getDev(devKey);
      console.log(devModel);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "Device does not exist" });
    }

    const userId = req.userId;
    const userKey = UserModel.getPkFromId(userId);
    console.log(`userKey ${userKey}`);
    let userModel: UserModel | undefined;
    try {
      userModel = await userAccess.getUser(userKey);
    } catch (error) {
      console.log(error);
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "User does not exist" });
    }

    devModel!.DEV_USER_KEY = userKey;

    try {
      await devAccess.updateDev(devModel!);
    } catch (error) {
      console.log(error);
      return res.status(StatusCodes.NOT_FOUND).json({
        message: `Could not pair user ${userKey} to device ${devKey}`,
      });
    }

    try {
      await pairingAccess.deletePairing(pairId);
    } catch (error) {
      console.log(error);
      return res.status(StatusCodes.NOT_FOUND).json({
        message: `Could not delete pair ${pairId}`,
      });
    }

    return res
      .status(StatusCodes.OK)
      .json({ message: `Paired user ${userKey} to device ${devKey}` });
  }
);

export default router;
