import { ExperimentContext } from '@/model/experiment-context';
import { baseURL, restClient } from "./base";

const apiBaseURL = `${baseURL}/roadnet`;

export class Roadnet {
  public static async genLoadRoadnetFromFile(folder: string, graphReaderType: string): Promise<ExperimentContext> {
    return (await restClient.put<ExperimentContext>(
      `${apiBaseURL}/loadfromfile/${folder}/${graphReaderType}`,
      {}
    )).data;
  }
}
