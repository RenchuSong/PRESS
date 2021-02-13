<template>
  <a-spin :tip="loadingTip" size="large" :spinning="spinning">
    <a-layout id="top-level-layout">
      <a-layout-header class="top-level-header">
        <div class="logo"><h1>PRESS</h1></div>
        <h3>A Novel Framework of Trajectory Compression in Road Networks</h3>
      </a-layout-header>
      <a-layout-content style="padding: 0 50px">
        <router-view name="routeHint" />
        <router-view name="content" />
      </a-layout-content>
    </a-layout>
  </a-spin>
</template>

<script lang="ts">
import { useStore } from "@/store";
import { computed, defineComponent, ref } from "vue";
export default defineComponent({
  name: "App",
  setup(_props, _context) {
    const store = useStore();
    return {
      loadingTip: computed(() => store.getters.currentLoadingJobs),
      spinning: computed(() => store.getters.isLoading),
    };
  },
});
</script>

<style lang="scss">
#top-level-layout {
  .logo {
    width: 160px;
    background: rgba(255, 255, 255, 0.2);
    margin: 0 28px 0 0;
    padding: 0;
    text-align: center;
    float: left;
  }

  .top-level-header {
    h1,
    h3 {
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
      color: #fff;
    }
  }

  .route-hint {
    margin: 16px 0;
  }

  .top-level-content-layout {
    padding: 24px 0;
    margin-bottom: 24px;
    background: #fff;
    min-height: calc(100vh - 144px);
  }

  .ant-card-hoverable {
    cursor: default;
  }
}

.ant-spin {
  position: fixed !important;
}

.section {
  border: 1px solid rgb(235, 237, 240);
}

.full-width {
  width: 100%;
}
</style>
