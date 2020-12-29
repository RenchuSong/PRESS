<template>
  <a-layout class="top-level-content-layout">
    <a-layout-content :style="{ padding: '0 24px', minHeight: '280px' }">
      <SearchBar
        v-model:searchQuery="searchQuery"
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
          <a-card>
            <template #cover>
              <img alt="example" src="@/assets/experiment-default.png" />
            </template>
            <a-card-meta :title="experiment.Name">
              <template #description>
                <div style="float: left">Created at&nbsp;</div>
                <div style="float: left">
                  {{
                    formatDatetime(experiment.CreationTime, "MM/DD HH:mm:ss")
                  }}
                </div>
              </template>
            </a-card-meta>
            <template class="ant-card-actions" #actions>
              <router-link :to="'/experiment/' + experiment.Id">
                <a-tooltip>
                  <template #title> Open experiment </template>
                  <folder-open-outlined key="open" />
                </a-tooltip>
              </router-link>
              <a-tooltip>
                <template #title> Delete experiment </template>
                <a-popconfirm
                  ok-text="Yes"
                  cancel-text="No"
                  @confirm="removeExperiment(experiment.Id)"
                >
                  <template #title>
                    Are you sure to delete this experiment?<br />
                    All progress will be lost and the operation is irreversible.
                  </template>
                  <delete-outlined key="delete" />
                </a-popconfirm>
              </a-tooltip>
            </template>
          </a-card>
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
import { DeleteOutlined, FolderOpenOutlined } from "@ant-design/icons-vue";

import CreateExperiment from "./experiments/CreateExperiment.vue";
import SearchBar from "./experiments/SearchBar.vue";
import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { defineComponent, onMounted } from "vue";
import Empty from "ant-design-vue/lib/empty";
import { formatDatetime } from "@/utility/utility";
import { ActionTypes } from "@/store/store-types";

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

    onMounted(() => {
      getExperiments();
      store.dispatch(ActionTypes.CLOSE_EXPERIMENT);
    });

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
      removeExperiment,
      simpleImage: Empty.PRESENTED_IMAGE_SIMPLE,
      formatDatetime,
    };
  },
  components: {
    DeleteOutlined,
    FolderOpenOutlined,
    CreateExperiment,
    SearchBar,
  },
});
</script>

<style scoped lang="scss">
.gutter-example ::v-deep(.ant-row > div) {
  background: transparent;
  border: 0;
}
.gutter-box {
  background: #00a0e9;
  padding: 5px 0;
}
.gutter-box-2 {
  background: #99a0e9;
  padding: 5px 0;
}

.experiment-search-icon {
  color: #6e6e6e;
  transition: all 0.3s cubic-bezier(0.645, 0.045, 0.355, 1);
  font-size: 16px;
}
</style>
