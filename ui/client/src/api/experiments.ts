import { ExperimentMeta } from '@/model/experiment_meta';
import { baseURL, restClient } from './base';

const apiBaseURL = `${baseURL}/experiments`;

export class Experiments {
  public static async genAllExperimentsMeta(): Promise<ExperimentMeta[]> {
    return (await restClient.get<ExperimentMeta[]>(`${apiBaseURL}/`)).data;
  }

  public static async genCreateExperiment(
    name: string,
  ): Promise<ExperimentMeta[]> {
    return (
      await restClient.post<ExperimentMeta[]>(
        `${apiBaseURL}/${name}`,
        {},
      )
    ).data;
  }

  public static async genRemoveExperiment(
    name: string,
  ): Promise<ExperimentMeta[]> {
    return (
      await restClient.delete<ExperimentMeta[]>(
        `${apiBaseURL}/${name}`,
      )
    ).data;
  }
}
