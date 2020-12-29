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
          <a-button type="primary" @click="showCreateExperimentModal" block>
            Create
          </a-button>
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
  <a-modal
    title="Create experiment"
    v-model:visible="createExperimentModalVisible"
    :confirm-loading="createExperimentModalConfirmLoading"
    @ok="handleCreateExperiment"
  >
    <a-form
      :model="newExperimentForm"
      :label-col="{ xs: { span: 24 }, sm: { span: 6 } }"
      :wrapper-col="{ xs: { span: 24 }, sm: { span: 16 } }"
    >
      <a-form-item label="Name">
        <a-input v-model:value="newExperimentForm.name" />
      </a-form-item>
      <a-form-item label="Cover image">
        <a-input v-model:value="newExperimentForm.image" />
      </a-form-item>
    </a-form>
  </a-modal>
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
import { defineComponent, ref } from "vue";
import Empty from "ant-design-vue/lib/empty";
import message from "ant-design-vue/lib/message";
import { RESTError } from "@/api/base";
import { formatDatetime } from "@/utility/utility";

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

    const { newExperimentForm, createExperiment } = useExperimentsCreate(store);

    const createExperimentModalVisible = ref(false);
    const createExperimentModalConfirmLoading = ref(false);
    const showCreateExperimentModal = () => {
      createExperimentModalVisible.value = true;
    };
    const handleCreateExperiment = async () => {
      createExperimentModalConfirmLoading.value = true;
      try {
        await createExperiment();
        createExperimentModalVisible.value = false;
      } catch (exception) {
        message.error((exception as RESTError).message);
      } finally {
        createExperimentModalConfirmLoading.value = false;
      }
    };

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
      removeExperiment,
      newExperimentForm,
      simpleImage: Empty.PRESENTED_IMAGE_SIMPLE,
      createExperimentModalVisible,
      createExperimentModalConfirmLoading,
      showCreateExperimentModal,
      handleCreateExperiment,
      formatDatetime,
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
