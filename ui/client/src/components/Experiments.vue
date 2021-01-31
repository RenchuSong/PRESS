<template>
  <a-layout class="top-level-content-layout">
    <a-layout-content :style="{ padding: '0 24px', minHeight: '280px' }">
      <SearchBar
        v-model:searchQuery="searchExperimentQuery"
        v-on:openCreateExperiment="
          $refs.createExperiment.startCreateExperiment()
        "
      />
      <a-divider type="horizontal" />
      <a-row
        type="flex"
        justify="start"
        :gutter="[
          { xs: 8, sm: 16, md: 24, lg: 32 },
          { xs: 8, sm: 16, md: 24, lg: 32 },
        ]"
      >
        <a-col
          :xs="24"
          :sm="12"
          :md="8"
          :xl="6"
          :xxl="4"
          v-for="experiment in experiments"
          :key="experiment.Id"
        >
          <ExperimentDisplayCard :experiment="experiment" />
        </a-col>
      </a-row>
      <a-empty v-if="experiments.length === 0" :image="simpleImage">
        <template #description>
          <span> No experiments </span>
        </template>
      </a-empty>
    </a-layout-content>
  </a-layout>
  <CreateExperiment ref="createExperiment" />
</template>

<script lang="ts">
import CreateExperiment from "./experiments/CreateExperiment.vue";
import SearchBar from "./experiments/SearchBar.vue";
import ExperimentDisplayCard from "./experiments/ExperimentDisplayCard.vue";
import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { defineComponent, onMounted } from "vue";
import Empty from "ant-design-vue/lib/empty";
import { RESTError } from "@/api/base";
import message from "ant-design-vue/lib/message";
import useExperiment from "@/composables/experiment/useExperiment";

export default defineComponent({
  name: "ExperimentsComponent",
  setup(_props, _context) {
    const store = useStore();

    const { closeExperiment } = useExperiment(store);

    const {
      experimentsCount,
      getExperiments,
      removeExperiment,
    } = useExperiments(store);

    const {
      searchExperimentQuery,
      experimentsMatchingSearchQuery,
    } = useExperimentsSearch(store);

    onMounted(async () => {
      closeExperiment();
      try {
        await getExperiments();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    });

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      searchExperimentQuery,
      removeExperiment,
      simpleImage: Empty.PRESENTED_IMAGE_SIMPLE,
    };
  },
  components: {
    CreateExperiment,
    SearchBar,
    ExperimentDisplayCard,
  },
});
</script>
