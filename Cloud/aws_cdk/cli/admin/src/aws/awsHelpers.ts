const devPrefix = "wootch_dev_";

/**
 * Concatene a predefined string with and id
 * @param id Id of the device
 * @returns a concatened name including the ID
 */
export function getThingNameFromId(id: string): string {
  return `${devPrefix.toUpperCase()}${id.toUpperCase()}`;
}

/**
 * Get a shortened Arn (for display purpose)
 * @param arn AWS Arn address
 * @returns shortned version of this Arn
 */
export function shortArn(arn: string): string {
  return "arn/" + arn.slice(40, 45);
}

/**
 * Get a shortened ID (for display purpose)
 * @param id any ID
 * * @returns shortned version of this Arn
 */
export function shortId(id: string): string {
  if (id.length < 5) throw new Error("id length");
  return id.slice(0, 5);
}

/**
 * Get a dynamodb primary key for a new user
 * @param id device ID
 * @returns dynamodb primary key
 */
export function devKeyFromId(id: string): string {
  return `DEV#${id.toUpperCase()}`;
}

/**
 * Get a dynamodb primary key for a new useri
 * @param id user ID
 * @returns dynamodb primary key
 */
export function userKeyFromId(id: string): string {
  return `USR#${id.toUpperCase()}`;
}
