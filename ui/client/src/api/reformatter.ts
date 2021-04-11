import { ExperimentContext } from '@/model/experiment-context';
import { GPSFolderSource } from '@/model/gps-folder-source';
import { PRESSTrajectoryData } from '@/model/press-trajectory';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/reformatter`;

export class Reformatter {
  public static async genListTrajectories(): Promise<string[]> {
    return (await restClient.get<string[]>(
      `${apiBaseURL}/list`,
      {}
    )).data;
  }

  public static async genPRESSTrajectory(id: string): Promise<PRESSTrajectoryData> {
    return (await restClient.get<PRESSTrajectoryData>(
      `${apiBaseURL}/press/${id}`,
      {}
    )).data;
  }

  public static async genDumpTrajectories(): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/dumptobinary`,
      {}
    )).message;
  }

  public static async genReformatTrajectories(): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/reformat`,
      {}
    )).message;
  }
}
