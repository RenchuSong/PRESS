<template>
  <a-button
    @click="
      handleLoadRoadnetFromFile(
        'WA_roadnetwork_and_single_trajectory',
        'SEATTLE_SAMPLE_ROADNET'
      )
    "
    >Load graph</a-button
  >
</template>

<script lang="ts">
import { defineComponent } from "vue";
import { useStore } from "@/store";
import useRoadnet from "@/composables/experiment/useRoadnet";
import { RESTError } from "@/api/base";
import message from "ant-design-vue/lib/message";

export default defineComponent({
  name: "Roadnet",
  setup(_props, _context) {
    const store = useStore();
    const { loadRoadnetFromFile } = useRoadnet(store);

    const handleLoadRoadnetFromFile = async (
      folder: string,
      graphReaderType: string
    ) => {
      try {
        await loadRoadnetFromFile(folder, graphReaderType);
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };

    return {
      handleLoadRoadnetFromFile,
    };
  },
});
</script>

<style scoped lang="scss">
</style>
