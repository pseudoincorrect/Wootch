import { NextFunction, Request, Response, Router } from "express";
import { StatusCodes } from "http-status-codes";
import { minAuth } from "../middlewares/authorizationMiddleware";
import { authGroups } from "../utils/authGroups";

const router = Router();

/******************************************************************************
 *  Get user data - "GET /api/user"
 ******************************************************************************/

router.get(
  "/",
  [minAuth(authGroups.users)],
  async (req: Request, res: Response, next: NextFunction) => {
    return res.status(StatusCodes.OK).json({ message: "Got to the users" });
  }
);

export default router;
