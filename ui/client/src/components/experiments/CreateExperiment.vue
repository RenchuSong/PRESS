<template>
  <a-modal
    title="Create experiment"
    v-model:visible="createExperimentModalVisible"
    :confirm-loading="createExperimentModalConfirmLoading"
    centered
    :closable="false"
    :maskClosable="false"
    @ok="handleCreateExperiment"
  >
    <a-form
      ref="theForm"
      :rules="rules"
      :model="newExperimentForm"
      :label-col="{ xs: { span: 24 }, sm: { span: 6 } }"
      :wrapper-col="{ xs: { span: 24 }, sm: { span: 16 } }"
    >
      <a-form-item label="Name" name="name">
        <a-input
          v-model:value="newExperimentForm.name"
          placeholder="Example: my experiment"
        />
      </a-form-item>
      <a-form-item label="Cover image" name="cover">
        <a-button @click="$refs.coverImageInput.click()">
          <UploadOutlined />Choose PNG image
        </a-button>
        <input
          type="file"
          accept=".png"
          @change="imageChanged"
          ref="coverImageInput"
          style="display: none"
        />
        <cropper
          v-if="imageBeforeCrop"
          class="cropper"
          :src="imageBeforeCrop"
          :stencil-props="{
            aspectRatio: 16 / 9,
          }"
          :resize-image="false"
          :move-image="false"
          @change="cropImage"
        ></cropper>
      </a-form-item>
    </a-form>
  </a-modal>
</template>

<script lang="ts">
import { UploadOutlined } from "@ant-design/icons-vue";

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
    cropImage(event: { canvas: { toDataURL: () => string } }) {
      this.newExperimentForm.image = event.canvas.toDataURL();
    },
    imageChanged(e: Event) {
      const fr = new FileReader();
      fr.onload = (event) => {
        this.imageBeforeCrop = (event.target as any).result as string;
      };
      fr.readAsDataURL((e.target as any)["files"][0]);
    },
    async handleCreateExperiment() {
      (this.$refs.theForm as any)
        .validate()
        .then(async () => {
          this.createExperimentModalConfirmLoading = true;
          try {
            const newExperimentMeta = await this.createExperiment();
            this.createExperimentModalVisible = false;
            router.push({
              path: `/experiment/${newExperimentMeta.id}`,
            });
          } catch (exception) {
            message.error((exception as RESTError).message);
          } finally {
            this.createExperimentModalConfirmLoading = false;
          }
        })
        .catch(() => message.error("Please fix all errors in the form."));
    },
  },
  components: {
    Cropper,
    UploadOutlined,
  },
  setup(props, context) {
    const store = useStore();
    const { newExperimentForm, createExperiment } = useExperimentsCreate(store);
    const createExperimentModalVisible = ref(false);
    const createExperimentModalConfirmLoading = ref(false);
    const startCreateExperiment = () => {
      createExperimentModalVisible.value = true;
    };
    const imageBeforeCrop = ref("");
    const imageAfterCrop = ref("");
    return {
      createExperiment,
      newExperimentForm,
      createExperimentModalVisible,
      createExperimentModalConfirmLoading,
      startCreateExperiment,
      imageBeforeCrop,
      imageAfterCrop,
      rules: {
        name: {
          required: true,
          message: "Please give the experiment a name",
        },
      },
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
