import { ExperimentMeta } from './experiment-meta';

export interface ExperimentContext extends ExperimentMeta {
  isOpen: string;
  id: number;
  name: string;
  roadnetReady: boolean;
  gridIndexReady: boolean;
  spTableReady: boolean;
};
