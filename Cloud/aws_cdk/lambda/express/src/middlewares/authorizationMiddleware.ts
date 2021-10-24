import { NextFunction, Request, RequestHandler, Response } from "express";
import { StatusCodes } from "http-status-codes";
import { authGroups } from "../utils/authGroups";

/**
 * Common claims for both id and access tokens
 */
export interface ClaimsBase {
  [name: string]: any;

  aud: string;
  iss: string;
  "cognito:groups"?: string[];
  exp: number;
  iat: number;
  sub: string;
  token_use: "id" | "access";
}

/**
 * Some id token specific claims
 */
export interface IdTokenClaims extends ClaimsBase {
  "cognito:username": string;
  email?: string;
  email_verified?: string;
  auth_time: string;
  token_use: "id";
}

/**
 * Some access token specific claims
 */
export interface AccessTokenClaims extends ClaimsBase {
  username?: string;
  token_use: "access";
}

/**
 * combined type for Claims
 */
export type Claims = IdTokenClaims | AccessTokenClaims;

// enrich the Express request type for type completion
declare global {
  namespace Express {
    interface Request {
      claims: Claims;
      groups: Set<string>;
      username: string;
      orgKey: string;
    }
  }
}

function getGroups(claims: ClaimsBase): Set<string> {
  const groups = claims["cognito:groups"];
  if (groups) {
    return new Set<string>(groups);
  }
  return new Set<string>();
}

function getClaimsFromToken(token?: string): Claims | null {
  if (!token) {
    return null;
  }
  try {
    const base64decoded = Buffer.from(token.split(".")[1], "base64").toString(
      "ascii"
    );
    return JSON.parse(base64decoded);
  } catch (e) {
    console.error("Invalid JWT token", e);
    return null;
  }
}

export interface ForceSignOutHandler {
  isForcedSignOut(req: Request): Promise<boolean>;
  forceSignOut(req: Request): Promise<void>;
}

export interface Opts {
  authorizationHeaderName: string;
  supportedGroups: string[];
  // forceSignOutHandler: ForceSignOutHandler;
  allowedPaths: string[];
}

export const authorizationMiddleware =
  (opts: Opts): RequestHandler =>
  async (req: Request, res: Response, next: NextFunction): Promise<any> => {
    if (opts.allowedPaths.includes(req.path)) {
      next();
      return;
    }

    const authorizationHeaderName =
      opts.authorizationHeaderName || "Authorization";

    const token = req.header(authorizationHeaderName);
    const claims = getClaimsFromToken(token);
    if (claims) req.claims = claims;

    if (claims) {
      if (claims["cognito:username"]) {
        req.username = claims["cognito:username"];
      } else if (claims.username) {
        req.username = claims.username;
      } else {
        console.warn(`No username claim found in token, using sub as username`);
        req.username = claims.sub;
      }

      if (claims["custom:orgKey"]) {
        req.orgKey = claims["custom:orgKey"];
      } else {
        res
          .status(StatusCodes.FORBIDDEN)
          .json({ error: "Unauthorized, no orgKey" });
        return;
      }

      const groups = getGroups(claims);
      req.groups = groups;

      const userHasAtLeastOneSupportedGroup = opts.supportedGroups.some((g) =>
        groups.has(g)
      );
      if (!userHasAtLeastOneSupportedGroup) {
        res.status(StatusCodes.FORBIDDEN).json({ error: "Unauthorized" });
        return;
      }

      // // check if user did a global sign out (optional)
      // const tokenIsRevoked = await opts.forceSignOutHandler.isForcedSignOut(req);
      // if (tokenIsRevoked) {
      //   res
      //     .status(401)
      //     .json({ error: "Your session has expired, please sign in again" });
      //   return;
      // }

      next();
    } else {
      res.status(StatusCodes.UNAUTHORIZED).json({ error: "Please sign in" });
    }
  };

export function isAuthorized(groups: Set<string>, level: string) {
  if (level == authGroups.admins) {
    return groups.has(authGroups.admins);
  } else if ((level = authGroups.managers)) {
    return groups.has(authGroups.admins) || groups.has(authGroups.managers);
  } else if ((level = authGroups.users)) {
    return (
      groups.has(authGroups.admins) ||
      groups.has(authGroups.managers) ||
      groups.has(authGroups.users)
    );
  }
  return false;
}

export const minAuth =
  (level: string): RequestHandler =>
  async (req: Request, res: Response, next: NextFunction): Promise<any> => {
    if (isAuthorized(req.groups, level)) {
      next();
      return;
    }
    res.status(StatusCodes.UNAUTHORIZED).json({ error: "Unauthorized" });
    return;
  };
