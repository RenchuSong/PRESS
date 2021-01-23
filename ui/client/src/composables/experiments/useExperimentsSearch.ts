import { Store } from "@/store";
import { computed, ref } from "vue";

export default function useExperimentsSearch(store: Store) {
  const searchExperimentQuery = ref("");
  const experimentsMatchingSearchQuery = computed(() => {
    return store.getters.experiments.filter(experiment =>
      experiment.name.includes(searchExperimentQuery.value)
    );
  });

  return {
    searchExperimentQuery,
    experimentsMatchingSearchQuery
  };
}
