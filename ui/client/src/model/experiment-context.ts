import { ExperimentMeta } from './experiment-meta';

export interface ExperimentContext extends ExperimentMeta {
  DataFolder: string;
  RoadnetReady: boolean;
};
