const patternSecret = RegExp(/^[A-Z0-9]*$/);

export const wootchThingNamePrefix = "WOOTCH_DEV_";
export const wootchDevKeyPrefix = "DEV#";
export const wootchPairKeyPrefix = "PAI#";

export function getEnv(variableName: string) {
  const variable = process.env[variableName];
  if (!variable) {
    throw new Error(`${variableName} environment variable must be defined`);
  }
  return variable;
}

export function devIdFromThingName(thingName: string) {
  return thingName.replace(wootchThingNamePrefix, "");
}
