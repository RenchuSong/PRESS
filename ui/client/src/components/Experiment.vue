<template>
  <a-layout class="top-level-content-layout">
    <a-layout-sider width="220">
      <NavBar />
    </a-layout-sider>
    <a-layout-content :style="{ padding: '0 24px', minHeight: '280px' }">
      <router-view />
    </a-layout-content>
  </a-layout>
</template>

<script lang="ts">
import useExperiment from "@/composables/experiment/useExperiment";
import { useStore } from "@/store";
import { defineComponent, onMounted } from "vue";
import message from "ant-design-vue/lib/message";
import { RESTError } from "@/api/base";
import NavBar from "./experiment/NavBar.vue";

export default defineComponent({
  name: "ExperimentComponent",
  props: {
    id: String,
  },
  setup(props, _context) {
    const store = useStore();
    const { openExperiment, getExperimentAuxiliaries } = useExperiment(store);
    onMounted(async () => {
      if (props.id && Number.isInteger(+props.id)) {
        try {
          await openExperiment(+props.id);
          await getExperimentAuxiliaries();
        } catch (exception) {
          message.error((exception as RESTError).message);
        }
      } else {
        message.error("Invalid experiment id.");
      }
    });

    return {};
  },
  components: {
    NavBar,
  },
});
</script>

<style scoped lang="scss">
</style>
