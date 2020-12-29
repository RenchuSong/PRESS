<template>
  <a-layout class="top-level-content-layout">
    <a-layout-sider width="160">
      <a-menu
        mode="inline"
        v-model:selectedKeys="selectedKeys2"
        v-model:openKeys="openKeys"
        style="height: 100%"
      >
        <a-sub-menu key="sub1">
          <template #title>
            <span><user-outlined />subnav 1</span>
          </template>
          <a-menu-item key="1">option1</a-menu-item>
          <a-menu-item key="2">option2</a-menu-item>
          <a-menu-item key="3">option3</a-menu-item>
          <a-menu-item key="4">option4</a-menu-item>
        </a-sub-menu>
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
    </a-layout-sider>
    <a-layout-content :style="{ padding: '0 24px', minHeight: '280px' }">
      Content
    </a-layout-content>
  </a-layout>
</template>

<script lang="ts">
import { StepBackwardOutlined } from "@ant-design/icons-vue";

import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { defineComponent } from "vue";

export default defineComponent({
  name: "ExperimentComponent",
  props: {
    id: String,
  },
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

    return {
      props,
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
      removeExperiment,
      routes: [
        {
          path: "index",
          breadcrumbName: "home",
        },
        {
          path: "first",
          breadcrumbName: "first",
          children: [
            {
              path: "/general",
              breadcrumbName: "General",
            },
            {
              path: "/layout",
              breadcrumbName: "Layout",
            },
            {
              path: "/navigation",
              breadcrumbName: "Navigation",
            },
          ],
        },
        {
          path: "second",
          breadcrumbName: "second",
        },
      ],
    };
  },
});
</script>

<style scoped lang="scss">
</style>
