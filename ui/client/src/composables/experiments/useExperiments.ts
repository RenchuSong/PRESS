import { Store } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useExperiments(store: Store) {
  const experiments = computed(() => store.getters.experiments);
  const experimentsCount = computed(() => store.getters.experimentsCount);

  const getExperiments = async () => {
    await store.dispatch(ActionTypes.GET_EXPERIMENTS);
  };

  const removeExperiment = async (id: number) => {
    await store.dispatch(ActionTypes.REMOVE_EXPERIMENTS, { id });
  };

  return {
    experiments,
    experimentsCount,
    getExperiments,
    removeExperiment
  };
}
