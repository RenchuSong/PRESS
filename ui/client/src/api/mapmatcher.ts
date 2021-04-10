import { ExperimentContext } from '@/model/experiment-context';
import { GPSFolderSource } from '@/model/gps-folder-source';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/mapmatcher`;

export class MapMatcher {
  public static async genGPSFolderSources(): Promise<GPSFolderSource[]> {
    return (await restClient.get<GPSFolderSource[]>(
      `${apiBaseURL}/foldersources`,
      {}
    )).data;
  }

  public static async genGPSFileSources(folder: string): Promise<string[]> {
    return (await restClient.get<string[]>(
      `${apiBaseURL}/filesources/${folder}`,
      {}
    )).data;
  }

  public static async genAddGPSAndMapMatch(
    sigmaZ: number,
    maxGPSBias: number,
    maxDistDifference: number,
    gpsTrajectoryReaderType: string,
    fileName: string
  ): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/add`,
      {
        sigmaZ,
        maxGPSBias,
        maxDistDifference,
        gpsTrajectoryReaderType,
        fileName
      }
    )).message;
  }

  public static async genLoadGridIndexFromBinary(): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfrombinary`,
      {}
    )).data;
  }
}
