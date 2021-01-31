<template>
  <a-button
    @click="
      loadRoadnetFromFile(
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

export default defineComponent({
  name: "Roadnet",
  setup(_props, _context) {
    const store = useStore();
    const { loadRoadnetFromFile } = useRoadnet(store);
    return {
      loadRoadnetFromFile,
    };
  },
});
</script>

<style scoped lang="scss">
</style>
