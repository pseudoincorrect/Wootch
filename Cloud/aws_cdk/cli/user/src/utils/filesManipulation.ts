import * as path from "path";
import * as fs from "fs";

export function deleteCredentials() {
  const filePath = getCredentialsFilePath();
  fs.rmSync(filePath, { force: true });
}

export function storeCredentials(creds: string) {
  const dirPath = getCredentialsDir();
  if (!fs.existsSync(dirPath)) {
    createCredentialsDir();
  }
  fs.writeFileSync(getCredentialsFilePath(), creds);
}

export function getCredentials(): string {
  const credPath = getCredentialsFilePath();
  if (!fs.existsSync(credPath)) {
    throw new Error("No credentials found");
  }
  const creds = fs.readFileSync(credPath, { encoding: "utf8", flag: "r" });
  return creds.toString();
}

export function createCredentialsDir() {
  const dirPath = getCredentialsDir();
  if (!fs.existsSync(dirPath)) {
    fs.mkdirSync(dirPath, { recursive: true });
  }
}

export function createLocalTmpDataDir() {
  const dirPath = getLocalTmpDataDirPath();
  if (!fs.existsSync(dirPath)) {
    fs.mkdirSync(dirPath, { recursive: true });
  }
}

export function getLocalTmpDataDirPath(): string {
  const appDataLocal =
    process.env.APPDATA ||
    (process.platform == "darwin"
      ? process.env.HOME + "/Library/Preferences"
      : process.env.HOME + "/.local/share");
  return path.join(appDataLocal, "wootch", "userCli");
}

export function getCredentialsDir(): string {
  const appDataLocal = getLocalTmpDataDirPath();
  return path.join(appDataLocal, "credentials");
}

export function getCredentialsFilePath(): string {
  return path.join(getCredentialsDir(), "idToken");
}
