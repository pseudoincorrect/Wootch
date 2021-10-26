import express = require("express");
import { Express, json, Request, Response, urlencoded } from "express";
import { eventContext } from "aws-serverless-express/middleware";
// import CognitoIdentityServiceProvider = require("aws-sdk/clients/cognitoidentityserviceprovider");
import { authorizationMiddleware } from "./middlewares/authorizationMiddleware";
import BaseRouter from "./routes/indexRoute";
import { AuthGroups, authGroups } from "./utils/authGroups";
import { StatusCodes } from "http-status-codes";
import { appErrorHandleMiddleware } from "./middlewares/errorMiddleware";

export interface AppOptions {
  authGroups: AuthGroups;
  authorizationHeaderName: string;
  userPoolId: string;
  // cognito: CognitoIdentityServiceProvider;
  // forceSignOutHandler: ForceSignOutHandler;
  expressApp?: Express; // intended for unit testing / mock purposes
}

export class App {
  constructor(
    private opts: AppOptions,
    public expressApp: Express = express()
  ) {
    const app = expressApp;

    app.use(json());
    app.use(urlencoded({ extended: true }));
    app.use(eventContext());
    app.use(
      authorizationMiddleware({
        authorizationHeaderName: opts.authorizationHeaderName,
        supportedGroups: Object.values(authGroups),
        // forceSignOutHandler: opts.forceSignOutHandler,
        allowedPaths: [],
      })
    );

    app.use("", BaseRouter);

    app.use(appErrorHandleMiddleware);
  }
}