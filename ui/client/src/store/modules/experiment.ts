import { Experiment } from "@/api/experiment";
import { GridIndex } from "@/api/grid-index";
import { Roadnet } from "@/api/roadnet";
import { AuxiliaryInfo } from "@/model/auxiliary-info";
import { ExperimentContext } from "@/model/experiment-context";
import { createRoadnet, RoadnetWithBound } from "@/model/roadnet";
import { RoadnetDataSource } from "@/model/roadnet-data-source";
import { formatFileSize } from "@/utility/utility";
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
  currentExperimentContext: undefined,
  roadnetReaderTypes: [],
  roadnetDataSources: [],
  currentExperimentAuxiliaries: [],
  currentExperimentStep: "roadnet",
} as ExperimentState);

const getters: GetterTree<ExperimentState, ExperimentState> &
  ExperimentGetters = {
  currentExperimentContext: state => state.currentExperimentContext,
  roadnetReaderTypes: state => state.roadnetReaderTypes,
  roadnetDataSources: state => state.roadnetDataSources,
  currentExperimentRoadnetBinaries:
    state => state.currentExperimentAuxiliaries.filter(
      value => value.filename === 'road_network.bin'
    ).map(
      value => `${value.filename} (${formatFileSize(value.size)})`
    ),
  currentExperimentGridIndexBinaries:
    state => state.currentExperimentAuxiliaries.filter(
      value => value.filename === 'grid_index.bin'
    ).map(
      value => `${value.filename} (${formatFileSize(value.size)})`
    ),
  currentExperimentStep: state => state.currentExperimentStep,
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
  },
  [ExperimentMutationTypes.SET_ROADNET_READER_TYPES](
    state: ExperimentState,
    payload: string[]
  ) {
    state.roadnetReaderTypes = payload;
  },
  [ExperimentMutationTypes.SET_ROADNET_DATA_SOURCES](
    state: ExperimentState,
    payload: RoadnetDataSource[]
  ) {
    state.roadnetDataSources = payload;
  },
  [ExperimentMutationTypes.SET_CURRENT_EXPERIMENT_AUXILIARIES](
    state: ExperimentState,
    payload: AuxiliaryInfo[]
  ) {
    state.currentExperimentAuxiliaries = payload;
  },
  [ExperimentMutationTypes.NAVIGATE_IN_EXPERIMENT](
    state: ExperimentState,
    payload: string
  ) {
    state.currentExperimentStep = payload;
  },
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
  async [ExperimentActionTypes.INIT_ROADNET_OPTIONS]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_ROADNET_READER_TYPES,
      await Roadnet.genReaderTypes()
    );
    commit(
      ExperimentMutationTypes.SET_ROADNET_DATA_SOURCES,
      await Roadnet.genDataSources()
    );
  },
  async [ExperimentActionTypes.GET_EXPERIMENT_AUXILIARIES]({ commit }, payload) {
    commit(
      ExperimentMutationTypes.SET_CURRENT_EXPERIMENT_AUXILIARIES,
      await Experiment.genAuxiliaries()
    );
  },
  async [ExperimentActionTypes.DUMP_ROADNET_TO_BINARY]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_CURRENT_EXPERIMENT_AUXILIARIES,
      await Roadnet.genDumpRoadnetToBinary()
    );
  },
  async [ExperimentActionTypes.LOAD_ROADNET_FROM_BINARY]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await Roadnet.genLoadRoadnetFromBinary()
    );
  },
  async [ExperimentActionTypes.BUILD_GRID_INDEX]({ commit }, payload) {
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await GridIndex.genBuildGridIndex(payload.width, payload.height)
    );
  },
  async [ExperimentActionTypes.DUMP_GRID_INDEX_TO_BINARY]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_CURRENT_EXPERIMENT_AUXILIARIES,
      await GridIndex.genDumpGridIndexToBinary()
    );
  },
  async [ExperimentActionTypes.LOAD_GRID_INDEX_FROM_BINARY]({ commit }) {
    commit(
      ExperimentMutationTypes.SET_EXPERIMENT_CONTEXT,
      await GridIndex.genLoadGridIndexFromBinary()
    );
  },
};

export default {
  state,
  getters,
  mutations,
  actions
};
