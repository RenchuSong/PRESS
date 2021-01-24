import { logger } from "@/utility/logger";
import axios, { AxiosRequestConfig, AxiosResponse } from "axios";

// Development.
export const baseURL = process.env.VUE_APP_POSTMAN_URL + "/api";
export const esURL = process.env.VUE_APP_POSTMAN_URL + "/subscribe";

// Production.
// export const baseURL = "/api";
// export const esURL = "/subscribe";

export interface RequestConfig {
  headers?: any;
  params?: any;
  timeout?: number;
  timeoutErrorMessage?: string;
}

export interface Response<T = any> {
  data: T;
  status: number;
  message: string;
}

export interface RESTError {
  status: number;
  message: string;
}

interface EventSourceData {
  requestID: string;
  code: number;
  message: string;
  data: any;
}

class RESTClient {
  evtSource: EventSource;
  requestMap: Map<string, {
    resolver: (value: Response | RESTError) => void,
    rejector: (value: Response | RESTError) => void
  }> = new Map();
  responseMap: Map<string, [Response | RESTError, boolean]> = new Map();

  public constructor() {
    this.evtSource = new EventSource(esURL);

    this.evtSource.addEventListener("response", event => {
      const data = JSON.parse((event as MessageEvent).data) as EventSourceData;
      // Hold the response.
      if (!this.requestMap.has(data.requestID)) {
        this.responseMap.set(data.requestID, this.parseSSEResponse(data));
        return;
      }

      // Resolve the request.
      const request = this.requestMap.get(data.requestID);
      if (request !== undefined) {
        const [result, success] = this.parseSSEResponse(data);
        if (success) {
          request.resolver(result);
        } else {
          request.rejector(result);
        }
        this.requestMap.delete(data.requestID)
      }
    });
    this.evtSource.onerror = function (err) {
      logger.error("EventSource error:", err);
    };
  }

  public async get<T>(
    url: string,
    config?: RequestConfig
  ): Promise<Response<T>> {
    return this.request<T>(url, "get", undefined, config);
  }

  public async post<T>(
    url: string,
    data: any,
    config?: RequestConfig
  ): Promise<Response<T>> {
    return this.request<T>(url, "post", data, config);
  }

  public async put<T>(
    url: string,
    data: any,
    config?: RequestConfig
  ): Promise<Response<T>> {
    return this.request<T>(url, "put", data, config);
  }

  public async delete<T>(
    url: string,
    config?: RequestConfig
  ): Promise<Response<T>> {
    return this.request<T>(url, "delete", undefined, config);
  }

  public async request<T>(
    url: string,
    method: string,
    data: any,
    config?: RequestConfig
  ): Promise<Response<T>> {
    try {
      let response: AxiosResponse;
      switch (method) {
        case "get": {
          response = await axios.get<T>(
            url,
            this.getAxiosRequestConfig(config)
          );
          break;
        }
        case "post": {
          response = await axios.post<T>(
            url,
            data,
            this.getAxiosRequestConfig(config)
          );
          break;
        }
        case "put": {
          response = await axios.put<T>(
            url,
            data,
            this.getAxiosRequestConfig(config)
          );
          break;
        }
        case "delete": {
          response = await axios.delete<T>(
            url,
            this.getAxiosRequestConfig(config)
          );
          break;
        }
        default: {
          throw new Error(`Not supported medhod ${method}`);
        }
      }
      const result = new Promise((resolve, reject) => {
        if (this.responseMap.has(response.headers['x-request-id'])) {
          // Already got the response, resolve or reject directly.
          const [resp, success] =
            this.responseMap.get(response.headers['x-request-id']) as
            [RESTError | Response<any>, boolean];
          if (success) {
            resolve(resp);
          } else {
            reject(resp);
          }
          this.responseMap.delete(response.headers['x-request-id']);
        } else {
          // Hold the request until get the response.
          this.requestMap.set(
            response.headers['x-request-id'],
            {
              resolver: resolve,
              rejector: reject
            }
          );
        }
      });
      return result as Promise<Response<T>>;
    } catch (err) {
      throw {
        status: err.response?.status || 500,
        message: err.code || "Server Error"
      } as RESTError;
    }
  }

  private getAxiosRequestConfig(
    config?: RequestConfig
  ): AxiosRequestConfig | undefined {
    if (config !== undefined) {
      return {
        headers: config.headers,
        params: config.params,
        timeout: config.timeout,
        timeoutErrorMessage: config.timeoutErrorMessage
      };
    }
  }

  private parseSSEResponse(
    data: EventSourceData
  ): [Response | RESTError, boolean] {
    if (data.code >= 200 && data.code < 300) {
      return [
        {
          data: data.data,
          status: data.code,
          message: data.message
        } as Response,
        true
      ];
    } else {
      return [
        {
          status: data.code,
          message: data.message
        } as RESTError,
        false
      ];
    }
  }
}

export const restClient = new RESTClient();
