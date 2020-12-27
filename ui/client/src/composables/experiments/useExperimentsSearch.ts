import { Store } from "@/store";
import { computed, ref } from "vue";

export default function useExperimentsSearch(store: Store) {
  const searchQuery = ref("");
  const experimentsMatchingSearchQuery = computed(() => {
    return store.state.experiments.experiments.filter(experiment =>
      experiment.Name.includes(searchQuery.value)
    );
  });

  return {
    searchQuery,
    experimentsMatchingSearchQuery
  };
}
