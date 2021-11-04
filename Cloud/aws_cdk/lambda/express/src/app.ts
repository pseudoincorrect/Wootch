import express = require("express");
import { Express, json, urlencoded } from "express";
import { eventContext } from "aws-serverless-express/middleware";
import { authorizationMiddleware } from "./middlewares/authorizationMiddleware";
import BaseRouter from "./routes/indexRoute";
import { AuthGroups, authGroups } from "./utils/authGroups";
import { appErrorHandleMiddleware } from "./middlewares/errorMiddleware";

export interface AppOptions {
  authGroups: AuthGroups;
  authorizationHeaderName: string;
  userPoolId: string;
  expressApp?: Express; // intended for unit testing / mock purposes
}

export class App {
  constructor(
    private opts: AppOptions,
    public expressApp: Express = express()
  ) {
    const app = expressApp;

    // requests are formated in JSON
    app.use(json());
    // exteded URL
    app.use(urlencoded({ extended: true }));
    app.use(eventContext());
    // Authorization middleware based on JWT token (cognito)
    app.use(
      authorizationMiddleware({
        authorizationHeaderName: opts.authorizationHeaderName,
        supportedGroups: Object.values(authGroups),
        allowedPaths: [],
      })
    );
    // load the routes
    app.use("", BaseRouter);
    // Error handling
    app.use(appErrorHandleMiddleware);
  }
}
