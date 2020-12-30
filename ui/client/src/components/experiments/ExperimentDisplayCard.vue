<template>
  <a-card>
    <template #cover>
      <img alt="Cover image" :src="experiment.Image" />
    </template>
    <a-card-meta :title="experiment.Name">
      <template #description>
        <div style="float: left">Created at&nbsp;</div>
        <div style="float: left">
          {{ formatDatetime(experiment.CreationTime, "MM/DD HH:mm:ss") }}
        </div>
      </template>
    </a-card-meta>
    <template #actions>
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
</template>

<script lang="ts">
import { DeleteOutlined, FolderOpenOutlined } from "@ant-design/icons-vue";

import useExperiments from "@/composables/experiments/useExperiments";
import { useStore } from "@/store";
import { defineComponent } from "vue";
import { formatDatetime } from "@/utility/utility";

export default defineComponent({
  name: "ExperimentsComponent",
  props: {
    experiment: Object,
  },
  setup(props, context) {
    const store = useStore();

    const { removeExperiment } = useExperiments(store);

    return {
      removeExperiment,
      formatDatetime,
    };
  },
  components: {
    DeleteOutlined,
    FolderOpenOutlined,
  },
});
</script>
