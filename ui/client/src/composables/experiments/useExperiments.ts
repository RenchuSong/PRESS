import { Store } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { computed, onMounted } from "vue";

export default function useExperiments(store: Store) {
  const experiments = computed(() => store.getters.experiments);
  const experimentsCount = computed(() => store.getters.experimentsCount);

  const getExperiments = async () => {
    await store.dispatch(ActionTypes.GET_EXPERIMENTS);
  };

  const removeExperiment = async (name: string) => {
    await store.dispatch(ActionTypes.REMOVE_EXPERIMENTS, { name });
  };

  onMounted(getExperiments);

  return {
    experiments,
    experimentsCount,
    getExperiments,
    removeExperiment
  };
}
