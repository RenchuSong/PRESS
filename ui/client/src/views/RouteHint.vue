<template>
  <a-breadcrumb class="route-hint">
    <a-breadcrumb-item>
      <router-link to="/"> <home-outlined /> Experiments </router-link>
    </a-breadcrumb-item>
    <a-breadcrumb-item v-if="experimentName">
      <experiment-outlined />
      <span>{{ experimentName }}</span>
    </a-breadcrumb-item>
    <template v-if="pathMatch">
      <a-breadcrumb-item v-for="(path, index) in pathMatch" :key="index">
        <span>{{ path }}</span>
      </a-breadcrumb-item>
    </template>
  </a-breadcrumb>
</template>

<script lang="ts">
import { computed, defineComponent } from "vue";
import { HomeOutlined, ExperimentOutlined } from "@ant-design/icons-vue";
import { useStore } from "@/store";

export default defineComponent({
  name: "RouteHint",
  props: {
    id: String,
    pathMatch: Array,
  },
  components: {
    HomeOutlined,
    ExperimentOutlined,
  },
  setup(props, context) {
    const store = useStore();
    const experimentName = computed(
      () => store.getters.currentExperimentContext?.name || undefined
    );
    return {
      experimentName,
    };
  },
});
</script>
