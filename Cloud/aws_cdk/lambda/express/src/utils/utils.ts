export class Utils {
  static getEnv(variableName: string) {
    const variable = process.env[variableName];
    if (!variable) {
      throw new Error(`${variableName} environment variable must be defined`);
    }
    return variable;
  }
}
