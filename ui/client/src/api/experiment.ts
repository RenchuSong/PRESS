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
}
