import { AuxiliaryInfo } from '@/model/auxiliary-info';
import { ExperimentContext } from '@/model/experiment-context';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/sptable`;

export class SPTable {
  public static async genBuildSPTable(distance: number): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/build/${distance}`,
      {}
    )).data;
  }

  public static async genDumpSPTableToBinary(): Promise<AuxiliaryInfo[]> {
    return (await restClient.put<AuxiliaryInfo[]>(
      `${apiBaseURL}/dumptobinary`,
      {}
    )).data;
  }

  public static async genLoadSPTableFromBinary(): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfrombinary`,
      {}
    )).data;
  }
}
