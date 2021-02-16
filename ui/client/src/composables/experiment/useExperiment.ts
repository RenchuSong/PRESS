import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";
import { RouteLocationNormalizedLoaded, Router } from "vue-router";

export default function useExperiment(store: Store) {
  const currentExperimentContext = computed(
    () => store.getters.currentExperimentContext
  );

  const openExperiment = async (id: number) => {
    store.commit(MutationTypes.NAVIGATE_IN_EXPERIMENT, "roadnet");
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.OPEN_EXPERIMENT,
      text: "Opening experiment"
    });
    await store.dispatch(ActionTypes.OPEN_EXPERIMENT, { id });
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.OPEN_EXPERIMENT
    });
  };

  const closeExperiment = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.CLOSE_EXPERIMENT,
      text: "Closing experiment"
    });
    await store.dispatch(ActionTypes.CLOSE_EXPERIMENT);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.CLOSE_EXPERIMENT
    });
  };

  const getExperimentAuxiliaries = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.GET_EXPERIMENT_AUXILIARIES,
      text: "Getting experiment auxiliaries"
    });
    await store.dispatch(ActionTypes.GET_EXPERIMENT_AUXILIARIES);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.GET_EXPERIMENT_AUXILIARIES
    });
  };

  const navigate = (
    route: RouteLocationNormalizedLoaded,
    router: Router,
    key: string
  ) => {
    store.commit(MutationTypes.NAVIGATE_IN_EXPERIMENT, key);
    const pathSplit = route.path.split("/");
    if (isNaN(+pathSplit[pathSplit.length - 1])) {
      pathSplit.pop();
    }
    if (key !== "roadnet") {
      pathSplit.push(key);
    }
    router.push(pathSplit.join("/"));
  };

  return {
    currentExperimentContext,
    openExperiment,
    closeExperiment,
    getExperimentAuxiliaries,
    navigate,
  };
}
