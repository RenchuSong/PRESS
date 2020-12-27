import {
  ExperimentActions,
  ExperimentActionTypes,
  ExperimentGetters,
  ExperimentMutations,
  ExperimentMutationTypes,
  ExperimentState
} from "./modules/experiment-types";
import {
  ExperimentsActions,
  ExperimentsActionTypes,
  ExperimentsGetters,
  ExperimentsMutations,
  ExperimentsMutationTypes,
  ExperimentsState
} from "./modules/experiments-types";

// state
export type RootStoreState = ExperimentsState & ExperimentState;
export type RootState = {
  experiments: ExperimentsState,
  experiment: ExperimentState
};

// mutations
export const MutationTypes = {
  ...ExperimentsMutationTypes,
  ...ExperimentMutationTypes
};
export type MutationTypes = typeof MutationTypes;
export type Mutations<S = RootState> = ExperimentsMutations<S> &
  ExperimentMutations<S>;

// actions
export const ActionTypes = {
  ...ExperimentsActionTypes,
  ...ExperimentActionTypes
};
export type ActionTypes = typeof ActionTypes;
export interface Actions extends ExperimentActions, ExperimentsActions { }

// getters
export type Getters = ExperimentsGetters & ExperimentGetters;
