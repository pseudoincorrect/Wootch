export class Utils {
  /**
   * Get the value of an enviroment variable
   * @param variableName
   * @returns assiciated value (environment variable)
   */
  static getEnv(variableName: string) {
    const variable = process.env[variableName];
    if (!variable) {
      throw new Error(`${variableName} environment variable must be defined`);
    }
    return variable;
  }
}
