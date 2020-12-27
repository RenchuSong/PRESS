import { Experiments } from "@/api/experiments";
import { ExperimentMeta } from "@/model/experiment-meta";
import { Store, useStore } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { computed, onMounted } from "vue";

export default function useExperiments(store: Store) {
  const experiments = computed(() => store.getters.experiments);
  const experimentsCount = computed(() => store.getters.experimentsCount);

  const getExperiments = async () => {
    await store.dispatch(ActionTypes.GET_EXPERIMENTS);
  };

  onMounted(getExperiments);

  return {
    experiments,
    experimentsCount,
    getExperiments
  };
}
