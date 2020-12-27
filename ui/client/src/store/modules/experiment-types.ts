import { ExperimentMeta } from "@/model/experiment-meta";
import { ActionContext } from "vuex";

// state
export interface ExperimentState {
  experiment: ExperimentMeta | undefined;
}

// mutations
export enum ExperimentMutationTypes {
  SET_EXPERIMENT = "SET_EXPERIMENT"
}

export type ExperimentMutations<S = ExperimentState> = {
  [ExperimentMutationTypes.SET_EXPERIMENT](
    state: S,
    payload: ExperimentMeta
  ): void;
};

// actions
export enum ExperimentActionTypes {
  GET_EXPERIMENT = "GET_EXPERIMENT"
}

type AugmentedExperimentActionContext = {
  commit<K extends keyof ExperimentMutations>(
    key: K,
    payload: Parameters<ExperimentMutations[K]>[1]
  ): ReturnType<ExperimentMutations[K]>;
} & Omit<ActionContext<ExperimentState, ExperimentState>, "commit">;

export interface ExperimentActions {
  [ExperimentActionTypes.GET_EXPERIMENT](
    { commit }: AugmentedExperimentActionContext,
    payload: ExperimentMeta
  ): Promise<void>;
}

// getters
export type ExperimentGetters = {};
