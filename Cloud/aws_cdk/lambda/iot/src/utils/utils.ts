const patternSecret = RegExp(/^[A-Z0-9]*$/);

export const wootchThingNamePrefix = "WOOTCH_DEV_";
export const wootchDevKeyPrefix = "DEV#";
export const wootchPairKeyPrefix = "PAI#";

/**
 * Get the value of an enviroment variable
 * @param variableName
 * @returns assiciated value (environment variable)
 */
export function getEnv(variableName: string) {
  const variable = process.env[variableName];
  if (!variable) {
    throw new Error(`${variableName} environment variable must be defined`);
  }
  return variable;
}

/**
 * extract the ID of a aws thing name
 * @param thingName AWS Iot-core thing name
 * @returns  alphanum ID
 */
export function devIdFromThingName(thingName: string) {
  return thingName.replace(wootchThingNamePrefix, "");
}
