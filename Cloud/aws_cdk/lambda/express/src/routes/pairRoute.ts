import { NextFunction, Request, Response, Router } from "express";
import { body, validationResult } from "express-validator";
import { StatusCodes } from "http-status-codes";

import { minAuth } from "../middlewares/authorizationMiddleware";
import { PairingRequestAccess } from "../storage/pairingRequestAccess";
import { AppValidationError } from "../utils/appErrors";
import { authGroups } from "../utils/authGroups";

const router = Router();
const pairingRequestAccess = new PairingRequestAccess();

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

    try {
      const pairModel = await pairingRequestAccess.getPairingRequest(
        req.body.secret
      );
      console.log(pairModel);
    } catch (error) {
      return res
        .status(StatusCodes.NOT_FOUND)
        .json({ message: "not a valid pairing request secret" });
    }

    return res
      .status(StatusCodes.OK)
      .json({ message: "Posted pairing attempt" });
  }
);

export default router;
