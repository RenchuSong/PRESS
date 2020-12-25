import { ExperimentMeta } from '@/model/experiment_meta';
import { restClient } from './base';

const baseURL = '/api/experiments/';

export class Experiments {
  public static async genAllExperimentsMeta(): Promise<ExperimentMeta[]> {
    return (await restClient.get<ExperimentMeta[]>(baseURL)).data;
  }

  public static async genCreateExperiment(
    name: string,
  ): Promise<ExperimentMeta[]> {
    return (await restClient.post<ExperimentMeta[]>(`baseURL${name}`, {})).data;
  }

  public static async genRemoveExperiment(
    name: string,
  ): Promise<ExperimentMeta[]> {
    return (await restClient.delete<ExperimentMeta[]>(`baseURL${name}`)).data;
  }
}
