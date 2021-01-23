class Logger {
  public info(...obj: any) {
    console.info(...obj);
  }

  public warn(...obj: any) {
    console.warn(...obj);
  }

  public error(...obj: any) {
    console.error(...obj);
  }
}

export const logger = new Logger();
