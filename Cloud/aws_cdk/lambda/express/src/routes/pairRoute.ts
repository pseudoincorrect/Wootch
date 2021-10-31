import { body, validationResult, query } from "express-validator";
import { NextFunction, Request, Response, Router } from "express";
import { StatusCodes } from "http-status-codes";
import { minAuth } from "../middlewares/authorizationMiddleware";
import { authGroups } from "../utils/authGroups";
import { AppValidationError } from "../utils/appErrors";

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
