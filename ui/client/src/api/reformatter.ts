import { ExperimentContext } from '@/model/experiment-context';
import { GPSFolderSource } from '@/model/gps-folder-source';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/reformatter`;

export class Reformatter {
  public static async genListTrajectories(): Promise<string[]> {
    return (await restClient.get<string[]>(
      `${apiBaseURL}/list`,
      {}
    )).data;
  }
}
