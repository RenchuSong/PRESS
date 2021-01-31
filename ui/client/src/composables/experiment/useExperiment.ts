import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useExperiment(store: Store) {
  const currentExperimentContext = computed(
    () => store.getters.currentExperimentContext
  );

  const openExperiment = async (id: number) => {
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

  return {
    currentExperimentContext,
    openExperiment,
    closeExperiment
  };
}
