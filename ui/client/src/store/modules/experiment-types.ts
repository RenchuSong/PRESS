import { AuxiliaryInfo } from '@/model/auxiliary-info';
import { ExperimentContext } from '@/model/experiment-context';
import { GPSFolderSource } from '@/model/gps-folder-source';
import { RoadnetDataSource } from '@/model/roadnet-data-source';
import { ActionContext } from "vuex";

// state
export interface ExperimentState {
  inExperiment: boolean;
  currentExperimentContext: ExperimentContext | undefined;
  roadnetReaderTypes: string[];
  roadnetDataSources: RoadnetDataSource[];
  currentExperimentAuxiliaries: AuxiliaryInfo[];
  currentExperimentStep: string;
  gpsFolderSources: GPSFolderSource[];
  gpsReaderTypes: string[];
  trajectories: string[];
}

// mutations
export enum ExperimentMutationTypes {
  TOGGLE_EXPERIMENT = "TOGGLE_EXPERIMENT",
  SET_EXPERIMENT_CONTEXT = "SET_EXPERIMENT_CONTEXT",
  SET_ROADNET_READER_TYPES = "SET_ROADNET_READER_TYPES",
  SET_ROADNET_DATA_SOURCES = "SET_ROADNET_DATA_SOURCES",
  SET_CURRENT_EXPERIMENT_AUXILIARIES = "SET_CURRENT_EXPERIMENT_AUXILIARIES",
  NAVIGATE_IN_EXPERIMENT = "NAVIGATE_IN_EXPERIMENT",
  SET_GPS_FOLDER_SOURCES = "SET_GPS_FOLDER_SOURCES",
  SET_GPS_READER_TYPES = "SET_GPS_READER_TYPES",
  SET_TRAJECTORIES = "SET_TRAJECTORIES",
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
  [ExperimentMutationTypes.SET_CURRENT_EXPERIMENT_AUXILIARIES](
    state: S,
    payload: AuxiliaryInfo[]
  ): void;
  [ExperimentMutationTypes.NAVIGATE_IN_EXPERIMENT](
    state: S,
    payload: string
  ): void;
  [ExperimentMutationTypes.SET_GPS_FOLDER_SOURCES](
    state: S,
    payload: GPSFolderSource[]
  ): void;
  [ExperimentMutationTypes.SET_GPS_READER_TYPES](
    state: S,
    payload: string[]
  ): void;
  [ExperimentMutationTypes.SET_TRAJECTORIES](
    state: S,
    payload: string[]
  ): void;
};

// actions
export enum ExperimentActionTypes {
  OPEN_EXPERIMENT = "OPEN_EXPERIMENT",
  CLOSE_EXPERIMENT = "CLOSE_EXPERIMENT",
  INIT_ROADNET_OPTIONS = "INIT_ROADNET_OPTIONS",
  LOAD_ROADNET_FROM_FILE = "LOAD_ROADNET_FROM_FILE",
  GET_EXPERIMENT_AUXILIARIES = "GET_EXPERIMENT_AUXILIARIES",
  DUMP_ROADNET_TO_BINARY = "DUMP_ROADNET_TO_BINARY",
  LOAD_ROADNET_FROM_BINARY = "LOAD_ROADNET_FROM_BINARY",
  UPDATE_ROADNET = "UPDATE_ROADNET", // Dummy, store cannot bear large object.
  BUILD_GRID_INDEX = "BUILD_GRID_INDEX",
  DUMP_GRID_INDEX_TO_BINARY = "DUMP_GRID_INDEX_TO_BINARY",
  LOAD_GRID_INDEX_FROM_BINARY = "LOAD_GRID_INDEX_FROM_BINARY",
  BUILD_SP_TABLE = "BUILD_SP_TABLE",
  DUMP_SP_TABLE_TO_BINARY = "DUMP_SP_TABLE_TO_BINARY",
  LOAD_SP_TABLE_FROM_BINARY = "LOAD_SP_TABLE_FROM_BINARY",
  INIT_GPS_FOLDER_OPTIONS = "INIT_GPS_FOLDER_OPTIONS",
  LIST_TRAJECTORIES = "LIST_TRAJECTORIES",
  PREVIEW_TRAJECTORY = "PREVIEW_TRAJECTORY", // Dummy, store cannot bear large object.
  LIST_GPS_FILES = "LIST_GPS_FILES", // Dummy
  CLEAR_GPS_FILES = "CLEAR_GPS_FILES", // Dummy
  MAPMATCH_GPS_FILES = "MAPMATCH_GPS_FILES", // Dummy
  REFORMAT_GPS_FILES = "REFORMAT_GPS_FILES", // Dummy
  DUMP_TRAJECTORIES = "DUMP_TRAJECTORIES", // Dummy
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
  [ExperimentActionTypes.GET_EXPERIMENT_AUXILIARIES](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.DUMP_ROADNET_TO_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.LOAD_ROADNET_FROM_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.BUILD_GRID_INDEX](
    { commit }: AugmentedExperimentActionContext,
    payload: { width: number, height: number }
  ): Promise<void>;
  [ExperimentActionTypes.DUMP_GRID_INDEX_TO_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.LOAD_GRID_INDEX_FROM_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.BUILD_SP_TABLE](
    { commit }: AugmentedExperimentActionContext,
    payload: { distance: number }
  ): Promise<void>;
  [ExperimentActionTypes.DUMP_SP_TABLE_TO_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.LOAD_SP_TABLE_FROM_BINARY](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.INIT_GPS_FOLDER_OPTIONS](
    { commit }: AugmentedExperimentActionContext,
    payload: {}
  ): Promise<void>;
  [ExperimentActionTypes.LIST_TRAJECTORIES](
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
  currentExperimentRoadnetBinaries(state: ExperimentState): string[];
  currentExperimentGridIndexBinaries(state: ExperimentState): string[];
  currentExperimentSPTableBinaries(state: ExperimentState): string[];
  currentExperimentStep(state: ExperimentState): string;
  gpsFolderSources(state: ExperimentState): GPSFolderSource[];
  gpsReaderTypes(state: ExperimentState): string[];
  trajectories(state: ExperimentState): string[];
};
