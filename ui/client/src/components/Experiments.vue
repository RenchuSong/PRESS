<template>
  <input v-model="searchQuery" />
  <div v-for="experiment in experiments" :key="experiment.Name">
    {{ experiment.Name }}
  </div>
  <div>{{ experimentsCount }}</div>
  <button @click.prevent="getExperiments">update</button>
</template>

<script lang="ts">
import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { ExperimentsMutationTypes } from "@/store/modules/experiments-types";
import { ActionTypes } from "@/store/store-types";
import { computed, defineComponent, reactive, ref, watch } from "vue";

export default defineComponent({
  name: "ExperimentsComponent",
  setup(props, context) {
    const store = useStore();

    const { experimentsCount, getExperiments } = useExperiments(store);

    const {
      searchQuery,
      experimentsMatchingSearchQuery,
    } = useExperimentsSearch(store);

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
    };
  },
});
</script>
