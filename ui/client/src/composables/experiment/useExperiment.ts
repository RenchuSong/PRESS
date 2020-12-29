import { Store } from "@/store";
import { ActionTypes } from "@/store/store-types";
import { computed } from "vue";

export default function useExperiment(store: Store) {
  const currentExperimentContext = computed(
    () => store.getters.currentExperimentContext
  );

  const openExperiment = async (id: number) => {
    await store.dispatch(ActionTypes.OPEN_EXPERIMENT, { id });
  };

  return {
    currentExperimentContext,
    openExperiment
  };
}
