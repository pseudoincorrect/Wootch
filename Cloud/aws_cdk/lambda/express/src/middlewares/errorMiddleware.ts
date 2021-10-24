import { NextFunction, Request, Response } from "express";
import { AppError } from "../utils/appErrors";

export function appErrorHandleMiddleware(
  err: any,
  req: Request,
  res: Response,
  next: NextFunction
) {
  if (err instanceof AppError) {
    console.error(err.toString());
    res.status(err.statusCode).send(err.messageToDisplay);
  } else {
    console.log("Non App Error");
    console.error(err);
    next(err);
  }
}
