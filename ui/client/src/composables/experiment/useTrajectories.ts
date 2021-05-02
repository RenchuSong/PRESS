import { MapMatcher } from "@/api/mapmatcher";
import { Reformatter } from "@/api/reformatter";
import { createGPSTrajectory, GPSTrajectoryWithBound } from "@/model/gps-trajectory";
import { createPRESSTrajectory, PRESSTrajectoryWithBound } from "@/model/press-trajectory";
import { Store } from "@/store";
import { MutationTypes, ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useTrajectories(store: Store) {
  const gpsReaderTypes = computed(() => store.getters.gpsReaderTypes);
  const gpsFolderSources = computed(() => store.getters.gpsFolderSources);
  const trajectories = computed(() => store.getters.trajectories);

  const loadTrajectories = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LIST_TRAJECTORIES,
      text: "Loading trajectories"
    });
    await store.dispatch(
      ActionTypes.LIST_TRAJECTORIES,
      {}
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LIST_TRAJECTORIES
    });
  };

  const initGPSFolderSources = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.INIT_GPS_FOLDER_OPTIONS,
      text: "Initializing GPS folder options"
    });
    await store.dispatch(
      ActionTypes.INIT_GPS_FOLDER_OPTIONS,
      {}
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.INIT_GPS_FOLDER_OPTIONS
    });
  };

  const getTrajectory = async (
    gpsFolder: string,
    pressFolder: string,
    id: string
  ): Promise<{
    gps: GPSTrajectoryWithBound,
    press: PRESSTrajectoryWithBound
  }> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.PREVIEW_TRAJECTORY,
      text: "Loading trajectory data to UI portal"
    });
    const gps = await MapMatcher.genGPSTrajectory(gpsFolder, id);
    const press = await Reformatter.genPRESSTrajectory(pressFolder, id);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.PREVIEW_TRAJECTORY
    });
    return {
      gps: createGPSTrajectory(gps),
      press: createPRESSTrajectory(press)
    };
  };

  const getGPSFilesInFolder = async (folder: string): Promise<string[]> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.LIST_GPS_FILES,
      text: "Get all GPS files"
    });
    const gpsFiles = await MapMatcher.genGPSFileSources(folder);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.LIST_GPS_FILES
    });
    return gpsFiles;
  };

  const clearGPSTrajectoriesInMemory = async (): Promise<void> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.CLEAR_GPS_FILES,
      text: "Clear GPS trajectories in memory"
    });
    await MapMatcher.genClearGPSTrajectories();
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.CLEAR_GPS_FILES
    });
  };

  const dumpTrajectories = async (
    gpsFolder: string,
    mmFolder: string,
    pressFolder: string
  ): Promise<void> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.DUMP_TRAJECTORIES,
      text: "Dump GPS, Map Matched, and PRESS trajectories to binary"
    });
    await MapMatcher.genDumpGPSTrajectories(gpsFolder);
    await MapMatcher.genDumpMapMatchedTrajectories(mmFolder);
    await Reformatter.genDumpTrajectories(pressFolder);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.DUMP_TRAJECTORIES
    });
  };

  const reformatTrajectories = async (): Promise<void> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.REFORMAT_GPS_FILES,
      text: "Reformat GPS trajectories to PRESS trajectories"
    });
    await Reformatter.genReformatTrajectories();
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.REFORMAT_GPS_FILES
    });
  };

  const mapMatch = async (
    sigmaZ: number,
    maxGPSBias: number,
    maxDistDifference: number,
    gpsTrajectoryReaderType: string,
    fileName: string
  ): Promise<void> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.MAPMATCH_GPS_FILES,
      text: `Map matching GPS trajectory: ${fileName}`
    });
    await MapMatcher.genMapMatch(
      sigmaZ,
      maxGPSBias,
      maxDistDifference,
      gpsTrajectoryReaderType,
      fileName
    );
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.MAPMATCH_GPS_FILES
    });
  };

  return {
    gpsReaderTypes,
    gpsFolderSources,
    trajectories,
    loadTrajectories,
    initGPSFolderSources,
    getTrajectory,

    getGPSFilesInFolder,
    clearGPSTrajectoriesInMemory,
    dumpTrajectories,
    reformatTrajectories,
    mapMatch,
  };
}
