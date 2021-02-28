import { AuxiliaryInfo } from '@/model/auxiliary-info';
import { ExperimentContext } from '@/model/experiment-context';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/gridindex`;

export class GridIndex {
  public static async genBuildGridIndex(width: number, height: number): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/build/${width}/${height}`,
      {}
    )).data;
  }

  public static async genDumpGridIndexToBinary(): Promise<AuxiliaryInfo[]> {
    return (await restClient.put<AuxiliaryInfo[]>(
      `${apiBaseURL}/dumptobinary`,
      {}
    )).data;
  }

  public static async genLoadGridIndexFromBinary(): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfrombinary`,
      {}
    )).data;
  }
}
