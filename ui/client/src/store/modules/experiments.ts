import { Experiment } from "@/api/experiment";
import { Experiments } from "@/api/experiments";
import { ExperimentMeta } from "@/model/experiment-meta";
import { ActionTree, GetterTree, MutationTree } from "vuex";
import {
  ExperimentsActions,
  ExperimentsActionTypes,
  ExperimentsGetters,
  ExperimentsMutations,
  ExperimentsMutationTypes,
  ExperimentsState
} from "./experiments-types";

const state: ExperimentsState = {
  experiments: []
};

const getters: GetterTree<ExperimentsState, ExperimentsState> &
  ExperimentsGetters = {
  experiments: state => state.experiments,
  experimentsCount: state => state.experiments.length
};

const mutations: MutationTree<ExperimentsState> & ExperimentsMutations = {
  [ExperimentsMutationTypes.SET_EXPERIMENTS](
    state: ExperimentsState,
    payload: ExperimentMeta[]
  ) {
    state.experiments = payload;
  }
};

const actions: ActionTree<ExperimentsState, ExperimentsState> &
  ExperimentsActions = {
  async [ExperimentsActionTypes.GET_EXPERIMENTS]({ commit }) {
    return commit(
      ExperimentsMutationTypes.SET_EXPERIMENTS,
      await Experiments.genAllExperimentsMeta()
    );
  },
  async [ExperimentsActionTypes.CREATE_EXPERIMENTS]({ commit }, payload) {
    const newExperimentMeta = await Experiments.genCreateExperiment(
      payload.name,
      payload.image
    );
    commit(ExperimentsMutationTypes.SET_EXPERIMENTS, [
      newExperimentMeta,
      ...state.experiments
    ]);
    return newExperimentMeta;
  },
  async [ExperimentsActionTypes.REMOVE_EXPERIMENTS]({ commit }, payload) {
    return commit(
      ExperimentsMutationTypes.SET_EXPERIMENTS,
      await Experiments.genRemoveExperiment(payload.id)
    );
  }
};

export default {
  state,
  getters,
  mutations,
  actions
};
