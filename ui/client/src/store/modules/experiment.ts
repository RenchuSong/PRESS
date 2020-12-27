import { ExperimentMeta } from "@/model/experiment-meta";
import { ActionTree, GetterTree, MutationTree } from "vuex";
import {
  ExperimentActions,
  ExperimentActionTypes,
  ExperimentGetters,
  ExperimentMutations,
  ExperimentMutationTypes,
  ExperimentState
} from "./experiment-types";

const state = () =>
  ({
    experiment: undefined
  } as ExperimentState);

const getters: GetterTree<ExperimentState, ExperimentState> &
  ExperimentGetters = {};

const mutations: MutationTree<ExperimentState> & ExperimentMutations = {
  [ExperimentMutationTypes.SET_EXPERIMENT](
    state: ExperimentState,
    payload: ExperimentMeta
  ) {
    state.experiment = payload;
  }
};

const actions: ActionTree<ExperimentState, ExperimentState> &
  ExperimentActions = {
  async [ExperimentActionTypes.GET_EXPERIMENT]({ commit }) {
    // commit(
    //   ExperimentMutationTypes.SET_EXPERIMENT,
    //   await Experiments.genAllExperimentsMeta()
    // );
  }
};

export default {
  state,
  getters,
  mutations,
  actions
};
