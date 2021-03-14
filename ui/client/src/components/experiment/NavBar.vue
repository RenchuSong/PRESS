<template>
  <a-menu
    mode="inline"
    style="height: 100%"
    v-model:selectedKeys="currentExperimentStep"
    @click="changeStep"
  >
    <a-menu-item key="roadnet">
      <span>
        <radar-chart-outlined />
        Roadnet
      </span>
    </a-menu-item>
    <a-menu-item key="gindexsptable" :disabled="!roadnetReady">
      <span>
        <global-outlined />
        Grid Index & SP Table
      </span>
    </a-menu-item>
    <a-menu-item key="gpstopress" :disabled="!gridIndexReady || !spTableReady">
      <span>
        <dash-outlined />
        GPS Traj to PRESS Traj
      </span>
    </a-menu-item>
    <a-sub-menu key="sub2">
      <template #title>
        <span><laptop-outlined />subnav 2</span>
      </template>
      <a-menu-item key="5">option5</a-menu-item>
      <a-menu-item key="6">option6</a-menu-item>
      <a-menu-item key="7">option7</a-menu-item>
      <a-menu-item key="8">option8</a-menu-item>
    </a-sub-menu>
    <a-sub-menu key="sub3">
      <template #title>
        <span><notification-outlined />subnav 3</span>
      </template>
      <a-menu-item key="9">option9</a-menu-item>
      <a-menu-item key="10">option10</a-menu-item>
      <a-menu-item key="11">option11</a-menu-item>
      <a-menu-item key="12">option12</a-menu-item>
    </a-sub-menu>
  </a-menu>
</template>

<script lang="ts">
import useExperiment from "@/composables/experiment/useExperiment";
import {
  GlobalOutlined,
  LaptopOutlined,
  NotificationOutlined,
  RadarChartOutlined,
  DashOutlined,
} from "@ant-design/icons-vue";
import { computed, defineComponent } from "vue";
import { useStore } from "@/store";

export default defineComponent({
  name: "NavBar",
  components: {
    GlobalOutlined,
    LaptopOutlined,
    NotificationOutlined,
    RadarChartOutlined,
    DashOutlined,
  },
  setup(_props, _context) {
    const store = useStore();
    const { navigate, currentExperimentContext } = useExperiment(store);
    return {
      roadnetReady: computed(
        () => currentExperimentContext.value?.roadnetReady
      ),
      gridIndexReady: computed(
        () => currentExperimentContext.value?.gridIndexReady
      ),
      spTableReady: computed(
        () => currentExperimentContext.value?.spTableReady
      ),
      navigate,
      currentExperimentStep: computed(() => [
        store.getters.currentExperimentStep,
      ]),
    };
  },
  methods: {
    changeStep({ key }: any) {
      this.navigate(this.$route, this.$router, key);
    },
  },
});
</script>

<style scoped lang="scss">
</style>
