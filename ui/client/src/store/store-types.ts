import {
  CommonGetters,
  CommonMutations,
  CommonMutationTypes,
  CommonState
} from "./modules/common-types";
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
export type RootStoreState = CommonState & ExperimentsState & ExperimentState;
export type RootState = {
  common: CommonState;
  experiments: ExperimentsState;
  experiment: ExperimentState;
};

// mutations
export const MutationTypes = {
  ...CommonMutationTypes,
  ...ExperimentsMutationTypes,
  ...ExperimentMutationTypes
};
export type MutationTypes = typeof MutationTypes;
export type Mutations<S = RootState> =
  CommonMutations<S> &
  ExperimentsMutations<S> &
  ExperimentMutations<S>;

// actions
export const ActionTypes = {
  ...ExperimentsActionTypes,
  ...ExperimentActionTypes
};
export type ActionTypes = typeof ActionTypes;
export interface Actions extends ExperimentActions, ExperimentsActions { };

// getters
export type Getters = CommonGetters & ExperimentsGetters & ExperimentGetters;
