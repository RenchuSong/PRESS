import { ExperimentMeta } from "@/model/experiment-meta";
import { ActionContext } from "vuex";

// state
export interface ExperimentsState {
  experiments: ExperimentMeta[];
}

// mutations
export enum ExperimentsMutationTypes {
  SET_EXPERIMENTS = "SET_EXPERIMENTS"
}

export type ExperimentsMutations<S = ExperimentsState> = {
  [ExperimentsMutationTypes.SET_EXPERIMENTS](
    state: S,
    payload: ExperimentMeta[]
  ): void;
};

// actions
export enum ExperimentsActionTypes {
  GET_EXPERIMENTS = "GET_EXPERIMENTS",
  CREATE_EXPERIMENTS = "CREATE_EXPERIMENTS",
  REMOVE_EXPERIMENTS = "REMOVE_EXPERIMENTS"
}

type AugmentedExperimentsActionContext = {
  commit<K extends keyof ExperimentsMutations>(
    key: K,
    payload: Parameters<ExperimentsMutations[K]>[1]
  ): ReturnType<ExperimentsMutations[K]>;
} & Omit<ActionContext<ExperimentsState, ExperimentsState>, "commit">;

export interface ExperimentsActions {
  [ExperimentsActionTypes.GET_EXPERIMENTS](
    { commit }: AugmentedExperimentsActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentsActionTypes.CREATE_EXPERIMENTS](
    { commit }: AugmentedExperimentsActionContext,
    payload: { name: string; image?: string }
  ): Promise<ExperimentMeta>;
  [ExperimentsActionTypes.REMOVE_EXPERIMENTS](
    { commit }: AugmentedExperimentsActionContext,
    payload: { id: number }
  ): Promise<void>;
}

// getters
export type ExperimentsGetters = {
  experiments(state: ExperimentsState): ExperimentMeta[];
  experimentsCount(state: ExperimentsState): number;
};
