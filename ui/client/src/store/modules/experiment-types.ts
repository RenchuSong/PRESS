import { ExperimentContext } from '@/model/experiment-context';
import { ActionContext } from "vuex";

// state
export interface ExperimentState {
  inExperiment: boolean;
  currentExperimentContext: ExperimentContext | undefined;
}

// mutations
export enum ExperimentMutationTypes {
  TOGGLE_EXPERIMENT = "TOGGLE_EXPERIMENT",
  SET_EXPERIMENT_CONTEXT = "SET_EXPERIMENT_CONTEXT"
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
};

// actions
export enum ExperimentActionTypes {
  OPEN_EXPERIMENT = "OPEN_EXPERIMENT",
  CLOSE_EXPERIMENT = "CLOSE_EXPERIMENT",
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
}

// getters
export type ExperimentGetters = {
  currentExperimentContext(state: ExperimentState):
    ExperimentContext | undefined;
};
