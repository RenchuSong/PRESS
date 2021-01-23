import { ExperimentMeta } from './experiment-meta';

export interface ExperimentContext extends ExperimentMeta {
  dataFolder: string;
  roadnetReady: boolean;
};
