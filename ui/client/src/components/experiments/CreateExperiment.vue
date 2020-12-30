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
      <input type="file" accept=".png" @change="imageChanged" />
      <cropper
        class="cropper"
        :src="imageBeforeCrop"
        :stencil-props="{
          aspectRatio: 16 / 9,
        }"
        :resize-image="false"
        :move-image="false"
        @change="cropImage"
      ></cropper>
    </a-form>
  </a-modal>
</template>

<script lang="ts">
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import { useStore } from "@/store";
import { defineComponent, ref } from "vue";
import message from "ant-design-vue/lib/message";
import { RESTError } from "@/api/base";
import router from "@/router";

import { Cropper } from "vue-advanced-cropper";
import "vue-advanced-cropper/dist/style.css";

export default defineComponent({
  name: "CreateExperiment",
  methods: {
    cropImage(event: Event) {
      console.log(event);
      // console.log((event.target as any).result as string);
      // this.imageAfterCrop = (this.$refs.cropper as any)
      //   .getCroppedCanvas()
      //   .toDataURL();
    },
    imageChanged(e: Event) {
      const fr = new FileReader();
      fr.onload = (event) => {
        this.imageBeforeCrop = (event.target as any).result as string;
      };
      fr.readAsDataURL((e.target as any)["files"][0]);
    },
  },
  components: {
    Cropper,
  },
  setup(props, context) {
    const store = useStore();
    const { newExperimentForm, createExperiment } = useExperimentsCreate(store);
    const createExperimentModalVisible = ref(true);
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

    const imageBeforeCrop = ref("");
    const imageAfterCrop = ref("");
    return {
      newExperimentForm,
      createExperimentModalVisible,
      createExperimentModalConfirmLoading,
      startCreateExperiment,
      handleCreateExperiment,
      imageBeforeCrop,
      imageAfterCrop,
    };
  },
});
</script>

<style scoped lang="scss">
.cropper {
  padding: 0;
  margin: 0;
  height: 200px;
  background: #ddd;
}
</style>
