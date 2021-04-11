import { MapMatcher } from "@/api/mapmatcher";
import { Reformatter } from "@/api/reformatter";
import { createGPSTrajectory, GPSTrajectoryWithBound } from "@/model/gps-trajectory";
import { createPRESSTrajectory, PRESSTrajectoryWithBound } from "@/model/press-trajectory";
import { Store } from "@/store";
import { MutationTypes, ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useTrajectories(store: Store) {
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

  const getTrajectory = async (id: string): Promise<{
    gps: GPSTrajectoryWithBound,
    press: PRESSTrajectoryWithBound
  }> => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.PREVIEW_TRAJECTORY,
      text: "Loading trajectory data to UI portal"
    });
    const gps = await MapMatcher.genGPSTrajectory(id);
    const press = await Reformatter.genPRESSTrajectory(id);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.PREVIEW_TRAJECTORY
    });
    return {
      gps: createGPSTrajectory(gps),
      press: createPRESSTrajectory(press)
    };
  };

  return {
    gpsFolderSources,
    trajectories,
    loadTrajectories,
    initGPSFolderSources,
    getTrajectory,
  };
}
