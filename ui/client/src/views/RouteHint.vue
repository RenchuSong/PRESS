<template>
  <a-breadcrumb class="route-hint">
    <a-breadcrumb-item>
      <router-link to="/"> <home-outlined /> Experiments </router-link>
    </a-breadcrumb-item>
    <a-breadcrumb-item v-if="experimentName">
      <experiment-outlined />
      <span>{{ experimentName }}</span>
    </a-breadcrumb-item>
  </a-breadcrumb>
</template>

<script lang="ts">
import { computed, defineComponent } from "vue";
import { HomeOutlined, ExperimentOutlined } from "@ant-design/icons-vue";
import { useStore } from "@/store";

export default defineComponent({
  name: "RouteHint",
  components: {
    HomeOutlined,
    ExperimentOutlined,
  },
  setup(props, context) {
    const store = useStore();
    const experimentName = computed(
      () => store.getters.currentExperimentContext?.Name || undefined
    );

    return {
      experimentName,
    };
  },
});
</script>
