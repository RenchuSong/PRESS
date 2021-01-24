import { Store } from "@/store";
import { ActionTypes, MutationTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useExperiments(store: Store) {
  const experiments = computed(() => store.getters.experiments);
  const experimentsCount = computed(() => store.getters.experimentsCount);

  const getExperiments = async () => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.GET_EXPERIMENTS,
      text: "Loading all experiments"
    });
    await store.dispatch(ActionTypes.GET_EXPERIMENTS);
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.GET_EXPERIMENTS
    });
  };

  const removeExperiment = async (id: number) => {
    store.commit(MutationTypes.START_JOB, {
      id: ActionTypes.REMOVE_EXPERIMENTS,
      text: "Deleting experiment"
    });
    await store.dispatch(ActionTypes.REMOVE_EXPERIMENTS, { id });
    store.commit(MutationTypes.FINISH_JOB, {
      id: ActionTypes.REMOVE_EXPERIMENTS
    });
  };

  return {
    experiments,
    experimentsCount,
    getExperiments,
    removeExperiment
  };
}
