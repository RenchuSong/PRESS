import { AuxiliaryInfo } from '@/model/auxiliary-info';
import { ExperimentContext } from '@/model/experiment-context';
import { RoadnetData } from '@/model/roadnet';
import { RoadnetDataSource } from '@/model/roadnet-data-source';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/roadnet`;

export class Roadnet {
  public static async genLoadRoadnetFromFile(folder: string, graphReaderType: string): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfromfile/${folder}/${graphReaderType}`,
      {}
    )).data;
  }

  public static async genReaderTypes(): Promise<string[]> {
    return (await restClient.get<string[]>(
      `${apiBaseURL}/readertypes`,
      {}
    )).data;
  }

  public static async genDataSources(): Promise<RoadnetDataSource[]> {
    return (await restClient.get<RoadnetDataSource[]>(
      `${apiBaseURL}/datasources`,
      {}
    )).data;
  }

  public static async genDumpRoadnetToBinary(): Promise<AuxiliaryInfo[]> {
    return (await restClient.put<AuxiliaryInfo[]>(
      `${apiBaseURL}/dumptobinary`,
      {}
    )).data;
  }

  public static async genLoadRoadnetFromBinary(): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfrombinary`,
      {}
    )).data;
  }

  public static async genRoadnet(): Promise<RoadnetData> {
    return (await restClient.get<RoadnetData>(
      `${apiBaseURL}`,
      {}
    )).data;
  }
}
