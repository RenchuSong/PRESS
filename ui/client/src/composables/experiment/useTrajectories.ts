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

  const initTrajectories = async () => {
    await loadTrajectories();
  }

  return {
    gpsFolderSources,
    trajectories,
    initTrajectories,
    loadTrajectories,
  };
}
