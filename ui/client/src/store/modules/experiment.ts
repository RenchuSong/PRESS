import { Experiment } from "@/api/experiment";
import { Roadnet } from "@/api/roadnet";
import { ExperimentContext } from "@/model/experiment-context";
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
  inExperiment: false,
  currentExperimentContext: undefined
} as ExperimentState);

const getters: GetterTree<ExperimentState, ExperimentState> &
  ExperimentGetters = {
  currentExperimentContext: state => state.currentExperimentContext,
};

const mutations: MutationTree<ExperimentState> & ExperimentMutations = {
  [ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT](
    state: ExperimentState,
    payload: ExperimentContext | undefined
  ) {
    if (state.inExperiment) {
      state.currentExperimentContext = payload;
    }
  },
  [ExperimentMutationTypes.TOGGLE_EXPERIMENT](
    state: ExperimentState,
    payload: boolean
  ) {
    state.inExperiment = payload;
  }
};

const actions: ActionTree<ExperimentState, ExperimentState> &
  ExperimentActions = {
  async [ExperimentActionTypes.OPEN_EXPERIMENT]({ commit }, payload) {
    commit(ExperimentMutationTypes.TOGGLE_EXPERIMENT, true);
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await Experiment.genOpenExperiment(payload.id)
    );
  },
  async [ExperimentActionTypes.CLOSE_EXPERIMENT]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await Experiment.genCloseExperiment()
    );
    commit(ExperimentMutationTypes.TOGGLE_EXPERIMENT, false);
  },
  async [ExperimentActionTypes.LOAD_ROADNET_FROM_FILE]({ commit }, payload) {
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await Roadnet.genLoadRoadnetFromFile(payload.folder, payload.graphReaderType)
    );
  },
};

export default {
  state,
  getters,
  mutations,
  actions
};
