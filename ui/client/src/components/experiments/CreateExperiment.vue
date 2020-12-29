<template>
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
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import { useStore } from "@/store";
import { defineComponent, ref } from "vue";
import message from "ant-design-vue/lib/message";
import { RESTError } from "@/api/base";
import { ActionTypes } from "@/store/store-types";
import router from "@/router";

export default defineComponent({
  name: "CreateExperiment",
  setup(props, context) {
    const store = useStore();
    const { newExperimentForm, createExperiment } = useExperimentsCreate(store);
    const createExperimentModalVisible = ref(false);
    const createExperimentModalConfirmLoading = ref(false);
    const startCreateExperiment = () => {
      createExperimentModalVisible.value = true;
    };
    const handleCreateExperiment = async () => {
      createExperimentModalConfirmLoading.value = true;
      try {
        const newExperimentMeta = await createExperiment();
        createExperimentModalVisible.value = false;
        router.push({
          name: "Experiment",
          params: { id: newExperimentMeta.Id },
        });
      } catch (exception) {
        message.error((exception as RESTError).message);
      } finally {
        createExperimentModalConfirmLoading.value = false;
      }
    };
    return {
      newExperimentForm,
      createExperimentModalVisible,
      createExperimentModalConfirmLoading,
      startCreateExperiment,
      handleCreateExperiment,
    };
  },
});
</script>
