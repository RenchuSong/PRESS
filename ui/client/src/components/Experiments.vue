<template>
  <input v-model="searchQuery" />
  <div>{{ experimentsCount }} experiments</div>
  <div v-for="experiment in experiments" :key="experiment.Name">
    {{ experiment.Name }} {{ experiment.CreationTime }} {{ experiment.Image }}
    <button @click.prevent="removeExperiment(experiment.Name)">delete</button>
  </div>
  <input v-model="newExperimentName" />
  <input v-model="newExperimentImage" />
  <button @click.prevent="createExperiment">add</button>
</template>

<script lang="ts">
import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { ExperimentsMutationTypes } from "@/store/modules/experiments-types";
import { ActionTypes } from "@/store/store-types";
import { computed, defineComponent, reactive, ref, watch } from "vue";

export default defineComponent({
  name: "ExperimentsComponent",
  setup(props, context) {
    const store = useStore();

    const {
      experimentsCount,
      getExperiments,
      removeExperiment,
    } = useExperiments(store);

    const {
      searchQuery,
      experimentsMatchingSearchQuery,
    } = useExperimentsSearch(store);

    const {
      newExperimentName,
      newExperimentImage,
      createExperiment,
    } = useExperimentsCreate(store);

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
      removeExperiment,
      newExperimentName,
      newExperimentImage,
      createExperiment,
    };
  },
});
</script>
