import { NextFunction, Request, Response, Router } from "express";
import { body, validationResult } from "express-validator";
import { StatusCodes } from "http-status-codes";

import { minAuth } from "../middlewares/authorizationMiddleware";
import { AppValidationError } from "../utils/appErrors";
import { authGroups } from "../utils/authGroups";

const router = Router();

/******************************************************************************
 *  POST Pairing attempt - "POST /api/pairing/attempt"
 ******************************************************************************/

router.post(
  "/attempt",
  [minAuth(authGroups.users)],
  body("secret").isAlphanumeric(),
  async (req: Request, res: Response, next: NextFunction) => {
    if (!validationResult(req).isEmpty()) {
      const msg = validationResult(req).array()[0].param;
      next(new AppValidationError(`POST user invalid params : ${msg}`));
      return;
    }
    return res
      .status(StatusCodes.OK)
      .json({ message: "Posted pairing attempt" });
  }
);

export default router;
