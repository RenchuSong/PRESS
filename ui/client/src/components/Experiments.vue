<template>
  <a-layout class="top-level-content-layout">
    <a-layout-content :style="{ padding: '0 24px', minHeight: '280px' }">
      <a-row type="flex" justify="center" :gutter="[16, 16]">
        <a-col :xs="24" :sm="16" :md="10" :lg="8" :xl="6" :xxl="5">
          <a-input
            class="experiment-search-wrapper"
            v-model:value="searchQuery"
            placeholder="Search experiments"
          >
            <template #suffix>
              <search-outlined class="experiment-search-icon" />
            </template>
          </a-input>
        </a-col>
        <a-col :xs="24" :sm="6" :md="5" :lg="4" :xl="3" :xxl="2">
          <a-button type="primary" block> Create </a-button>
        </a-col>
      </a-row>
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
              <img
                alt="example"
                src="https://media.sproutsocial.com/uploads/2017/08/Social-Media-Video-Specs-Feature-Image.png"
              />
            </template>
            <a-card-meta :title="experiment.Name">
              <template #description>{{ experiment.CreationTime }}</template>
            </a-card-meta>
            <template class="ant-card-actions" #actions>
              <a-tooltip>
                <template #title> Open experiment </template>
                <folder-open-outlined key="open" />
              </a-tooltip>
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
      <input v-model="newExperimentName" />
      <input v-model="newExperimentImage" />
      <button @click.prevent="createExperiment">add</button>
    </a-layout-content>
  </a-layout>
</template>

<script lang="ts">
import {
  DeleteOutlined,
  FolderOpenOutlined,
  SearchOutlined,
} from "@ant-design/icons-vue";

import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { defineComponent } from "vue";
import Empty from "ant-design-vue/lib/empty";

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
      simpleImage: Empty.PRESENTED_IMAGE_SIMPLE,
    };
  },
  components: {
    DeleteOutlined,
    FolderOpenOutlined,
    SearchOutlined,
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
