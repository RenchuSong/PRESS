import { AuxiliaryInfo } from '@/model/auxiliary-info';
import { ExperimentContext } from '@/model/experiment-context';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/experiment`;

export class Experiment {
  public static async genOpenExperiment(id: number): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/open/${id}`,
      {}
    )).data;
  }

  public static async genCloseExperiment(): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/close`,
      {}
    )).data;
  }

  public static async genAuxiliaries(): Promise<AuxiliaryInfo[]> {
    return (await restClient.get<AuxiliaryInfo[]>(
      `${apiBaseURL}/auxiliaries`,
      {}
    )).data;
  }
}
