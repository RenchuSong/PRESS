import { ExperimentContext } from '@/model/experiment-context';
import { RoadnetDataSource } from '@/model/roadnet-data-source';
import { ActionContext } from "vuex";

// state
export interface ExperimentState {
  inExperiment: boolean;
  currentExperimentContext: ExperimentContext | undefined;
  roadnetReaderTypes: string[];
  roadnetDataSources: RoadnetDataSource[];
}

// mutations
export enum ExperimentMutationTypes {
  TOGGLE_EXPERIMENT = "TOGGLE_EXPERIMENT",
  SET_EXPERIMENT_CONTEXT = "SET_EXPERIMENT_CONTEXT",
  SET_ROADNET_READER_TYPES = "SET_ROADNET_READER_TYPES",
  SET_ROADNET_DATA_SOURCES = "SET_ROADNET_DATA_SOURCES",
}

export type ExperimentMutations<S = ExperimentState> = {
  [ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT](
    state: S,
    payload: ExperimentContext | undefined
  ): void;
  [ExperimentMutationTypes.TOGGLE_EXPERIMENT](
    state: S,
    payload: boolean
  ): void;
  [ExperimentMutationTypes.SET_ROADNET_READER_TYPES](
    state: S,
    payload: string[]
  ): void;
  [ExperimentMutationTypes.SET_ROADNET_DATA_SOURCES](
    state: S,
    payload: RoadnetDataSource[]
  ): void;
};

// actions
export enum ExperimentActionTypes {
  OPEN_EXPERIMENT = "OPEN_EXPERIMENT",
  CLOSE_EXPERIMENT = "CLOSE_EXPERIMENT",
  INIT_ROADNET_OPTIONS = "INIT_ROADNET_OPTIONS",
  LOAD_ROADNET_FROM_FILE = "LOAD_ROADNET_FROM_FILE",
}

type AugmentedExperimentActionContext = {
  commit<K extends keyof ExperimentMutations>(
    key: K,
    payload: Parameters<ExperimentMutations[K]>[1]
  ): ReturnType<ExperimentMutations[K]>;
} & Omit<ActionContext<ExperimentState, ExperimentState>, "commit">;

export interface ExperimentActions {
  [ExperimentActionTypes.OPEN_EXPERIMENT](
    { commit }: AugmentedExperimentActionContext,
    payload: { id: number }
  ): Promise<void>;
  [ExperimentActionTypes.CLOSE_EXPERIMENT](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.LOAD_ROADNET_FROM_FILE](
    { commit }: AugmentedExperimentActionContext,
    payload: { folder: string, graphReaderType: string }
  ): Promise<void>;
  [ExperimentActionTypes.INIT_ROADNET_OPTIONS](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
}

// getters
export type ExperimentGetters = {
  currentExperimentContext(state: ExperimentState):
    ExperimentContext | undefined;
  roadnetReaderTypes(state: ExperimentState): string[];
  roadnetDataSources(state: ExperimentState): RoadnetDataSource[];
};
