import { ExperimentContext } from '@/model/experiment-context';
import { GPSFolderSource } from '@/model/gps-folder-source';
import { GPSTrajectoryData } from '@/model/gps-trajectory';
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

  public static async genReaderTypes(): Promise<string[]> {
    return (await restClient.get<string[]>(
      `${apiBaseURL}/readertypes`,
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

  public static async genLoadGridIndexFromBinary(folder: string): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfrombinary/${folder}`,
      {}
    )).data;
  }

  public static async genGPSTrajectory(folder: string, id: string): Promise<GPSTrajectoryData> {
    return (await restClient.get<GPSTrajectoryData>(
      `${apiBaseURL}/gps/${folder}/${id}`,
      {}
    )).data;
  }

  public static async genClearGPSTrajectories(): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/clear`,
      {}
    )).message;
  }

  public static async genDumpGPSTrajectories(folder: string): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/dumpgpstobinary/${folder}`,
      {}
    )).message;
  }

  public static async genDumpMapMatchedTrajectories(folder: string): Promise<string> {
    return (await restClient.put<string>(
      `${apiBaseURL}/dumptobinary/${folder}`,
      {}
    )).message;
  }

  public static async genMapMatch(
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
}
