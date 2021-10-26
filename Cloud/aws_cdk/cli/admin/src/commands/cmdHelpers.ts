import { v4 as uuidv4 } from "uuid";

export function getAnId(): string {
  return uuidv4().replace(/-/g, "").toUpperCase();
}
