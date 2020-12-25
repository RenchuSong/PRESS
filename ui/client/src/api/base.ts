import axios, { AxiosRequestConfig, AxiosResponse } from 'axios';

export interface RequestConfig {
  headers?: any;
  params?: any;
  timeout?: number;
  timeoutErrorMessage?: string;
}

export interface Response<T = any> {
  error: boolean;
  data?: T;
  status: number;
  message: string;
}

// TODO: use sse to decouple request and response form API to avoid
// browser 1 minute retry "feature".
export class RESTClient {
  public async get<T>(
    url: string,
    config?: RequestConfig,
  ): Promise<Response<T>> {
    return this.request<T>(url, 'get', undefined, config);
  }

  public async post<T>(
    url: string,
    data: any,
    config?: RequestConfig,
  ): Promise<Response<T>> {
    return this.request<T>(url, 'post', data, config);
  }

  public async put<T>(
    url: string,
    data: any,
    config?: RequestConfig,
  ): Promise<Response<T>> {
    return this.request<T>(url, 'put', data, config);
  }

  public async delete<T>(
    url: string,
    config?: RequestConfig,
  ): Promise<Response<T>> {
    return this.request<T>(url, 'delete', undefined, config);
  }

  public async request<T>(
    url: string,
    method: string,
    data: any,
    config?: RequestConfig,
  ): Promise<Response<T>> {
    try {
      let response: AxiosResponse;
      switch (method) {
        case 'get': {
          response = await axios.get<T>(
            url,
            this.getAxiosRequestConfig(config),
          );
          break;
        }
        case 'post': {
          response = await axios.post<T>(
            url,
            data,
            this.getAxiosRequestConfig(config),
          );
        }
        case 'put': {
          response = await axios.put<T>(
            url,
            data,
            this.getAxiosRequestConfig(config),
          );
        }
        case 'delete': {
          response = await axios.delete<T>(
            url,
            this.getAxiosRequestConfig(config),
          );
          break;
        }
        default: {
          throw new Error(`Not supported medhod ${method}`);
        }
      }
      return {
        error: false,
        data: response.data,
        status: response.status,
        message: response.statusText,
      };
    } catch (err) {
      return {
        error: true,
        status: err.response?.status || 500,
        message: err.code || 'Server Error',
      }
    }
  }

  private getAxiosRequestConfig(
    config?: RequestConfig,
  ): AxiosRequestConfig | undefined {
    if (config !== undefined) {
      return {
        headers: config.headers,
        params: config.params,
        timeout: config.timeout,
        timeoutErrorMessage: config.timeoutErrorMessage,
      };
    }
  }
}
