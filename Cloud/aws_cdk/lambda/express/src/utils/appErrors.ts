import { StatusCodes, getReasonPhrase } from "http-status-codes";
import PrettyError from "pretty-error";

var pe = new PrettyError();
pe.withoutColors();

interface AppErrorConf {
  statusCode: number;
  messageToDisplay: string;
  errorType: string;
  devMessage?: string;
  originError?: Error;
}

/**
 * Class that provide a concatenation of application specific error
 * and non-app error (node and packages). It also associate a http
 * error status code and display the message nicely.
 */
export abstract class AppError extends Error {
  private errorType: string;
  public statusCode: number;
  public messageToDisplay: string;
  public devMessage?: string;
  public orginError?: Error;

  constructor(conf: AppErrorConf) {
    super(conf.messageToDisplay);
    this.errorType = conf.errorType;
    this.statusCode = conf.statusCode;
    this.messageToDisplay = conf.messageToDisplay;
    if (conf.devMessage) this.devMessage = conf.devMessage;
    if (conf.originError) {
      this.orginError = conf.originError;
    }
  }

  toString(): string {
    let devMessage = "";
    let orginError = "";
    if (this.devMessage) devMessage = "!! DEV MESSAGE !! " + this.devMessage;
    if (this.orginError)
      orginError = "!! ORIGIN ERROR !! " + pe.render(this.orginError);

    return (
      `
    
    !! STATUS CODE !! [${this.statusCode}] ${getReasonPhrase(this.statusCode)} 

    !! APP ERROR: ${this.errorType} !! ${pe.render(this)} ` +
      (orginError ? "\n" : "") +
      orginError +
      (devMessage ? "\n" : "") +
      devMessage
    );
  }
}

export class AppDbError extends AppError {
  constructor(message: string, orginError?: Error, devMesage?: string) {
    super({
      errorType: "DYNAMODB ERROR",
      statusCode: StatusCodes.INTERNAL_SERVER_ERROR,
      messageToDisplay: message,
      originError: orginError,
      devMessage: devMesage,
    });
  }
}

export class AppNotFoundError extends AppError {
  constructor(message: string, orginError?: Error, devMesage?: string) {
    super({
      errorType: "NOT FOUND ERROR",
      statusCode: StatusCodes.NOT_FOUND,
      messageToDisplay: message,
      originError: orginError,
      devMessage: devMesage,
    });
  }
}

export class AppValidationError extends AppError {
  constructor(message: string, orginError?: Error, devMesage?: string) {
    super({
      errorType: "VALIDATION ERROR",
      statusCode: StatusCodes.BAD_REQUEST,
      messageToDisplay: message,
      originError: orginError,
      devMessage: devMesage,
    });
  }
}

export class AppForbiddenError extends AppError {
  constructor(message: string, orginError?: Error, devMesage?: string) {
    super({
      errorType: "FORBIDDEN ERROR",
      statusCode: StatusCodes.FORBIDDEN,
      messageToDisplay: message,
      originError: orginError,
      devMessage: devMesage,
    });
  }
}
