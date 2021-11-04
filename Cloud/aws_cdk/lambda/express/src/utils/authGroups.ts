import { Utils } from "./utils";

export interface AuthGroups {
  users: string;
  admins: string;
}

/**
 * Groups uppon which the application associate the privileges
 */
export const authGroups: AuthGroups = {
  admins: Utils.getEnv("ADMINS_GROUP_NAME"),
  users: Utils.getEnv("USERS_GROUP_NAME"),
};
