<template>
  <input v-model="searchQuery" />
  <div>{{ experimentsCount }} experiments</div>
  <a-row type="flex" justify="start" :gutter="[16, 16]">
    <a-col
      :xs="24"
      :sm="12"
      :md="8"
      :lg="6"
      :xl="4"
      :xxl="3"
      v-for="experiment in experiments"
      :key="experiment.Name"
    >
      <a-card hoverable>
        <template #cover>
          <img
            alt="example"
            src="https://play-lh.googleusercontent.com/IeNJWoKYx1waOhfWF6TiuSiWBLfqLb18lmZYXSgsH1fvb8v1IYiZr5aYWe0Gxu-pVZX3"
          />
        </template>
        <a-card-meta :title="experiment.Name">
          <template #description>{{ experiment.CreationTime }}</template>
        </a-card-meta>
      </a-card>
    </a-col>
  </a-row>
  <div v-for="experiment in experiments" :key="experiment.Name">
    {{ experiment.Name }} {{ experiment.CreationTime }} {{ experiment.Image }}
    <button @click.prevent="removeExperiment(experiment.Name)">delete</button>
  </div>
  <input v-model="newExperimentName" />
  <input v-model="newExperimentImage" />
  <button @click.prevent="createExperiment">add</button>
  <a-row>
    <a-col :span="12"><p class="height-200">col-4</p></a-col>
    <a-col :span="12">
      <a-button block>Cancel</a-button>
      <a-button type="primary" block>
        <template #icon>
          <StepBackwardOutlined />
          Test
        </template>
      </a-button>
    </a-col>
  </a-row>
  <a-divider orientation="center"> Fill rest </a-divider>
  <a-row type="flex">
    <a-col :span="6" :order="4"> 1 col-order-4 </a-col>
    <a-col :span="6" :order="3"> 2 col-order-3 </a-col>
    <a-col :span="6" :order="2"> 3 col-order-2 </a-col>
    <a-col :span="6" :order="1"> 4 col-order-1 </a-col>
  </a-row>
  <a-row :gutter="[16, { xs: 8, sm: 16, md: 24, lg: 32 }]">
    <a-col class="gutter-row" :span="6">
      <div class="gutter-box">col-6</div>
    </a-col>
    <a-col class="gutter-row" :span="6">
      <div class="gutter-box">col-6</div>
    </a-col>
    <a-col class="gutter-row" :span="6">
      <div class="gutter-box">col-6</div>
    </a-col>
    <a-col class="gutter-row" :span="6">
      <div class="gutter-box">col-6</div>
    </a-col>
    <a-col class="gutter-row" :span="6">
      <div class="gutter-box">col-6</div>
    </a-col>
  </a-row>
  <a-row>
    <a-col
      class="gutter-box"
      :xs="{ span: 5, offset: 1 }"
      :lg="{ span: 6, offset: 2 }"
    >
      Col
    </a-col>
    <a-col
      class="gutter-box"
      :xs="{ span: 11, offset: 1 }"
      :lg="{ span: 6, offset: 2 }"
    >
      Col
    </a-col>
    <a-col
      class="gutter-box"
      :xs="{ span: 5, offset: 1 }"
      :lg="{ span: 6, offset: 2 }"
    >
      Col
    </a-col>
  </a-row>
  <a-row>
    <a-col class="gutter-box" :span="18" :pull="12"> col-18 col-push-6 </a-col>
    <a-col class="gutter-box-2" :span="6" :pull="2"> col-6 col-pull-18 </a-col>
  </a-row>
  <a-space>
    Space
    <a-button type="primary">Button</a-button>
    <a-upload>
      <a-button> Click to Upload </a-button>
    </a-upload>
    <a-popconfirm
      title="Are you sure delete this task?"
      ok-text="Yes"
      cancel-text="No"
    >
      <a-button>Confirm</a-button>
    </a-popconfirm>
  </a-space>
  <!-- <a-button-group> -->

  <!-- </a-button-group> -->
  <a-date-picker />
</template>

<script lang="ts">
import { StepBackwardOutlined } from "@ant-design/icons-vue";

import useExperiments from "@/composables/experiments/useExperiments";
import useExperimentsCreate from "@/composables/experiments/useExperimentsCreate";
import useExperimentsSearch from "@/composables/experiments/useExperimentsSearch";
import { useStore } from "@/store";
import { defineComponent } from "vue";

export default defineComponent({
  name: "ExperimentsComponent",
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

    const {
      newExperimentName,
      newExperimentImage,
      createExperiment,
    } = useExperimentsCreate(store);

    return {
      experiments: experimentsMatchingSearchQuery,
      experimentsCount,
      getExperiments,
      searchQuery,
      removeExperiment,
      newExperimentName,
      newExperimentImage,
      createExperiment,
    };
  },
  components: {
    StepBackwardOutlined,
  },
});
</script>

<style scoped lang="scss">
.gutter-example ::v-deep(.ant-row > div) {
  background: transparent;
  border: 0;
}
.gutter-box {
  background: #00a0e9;
  padding: 5px 0;
}
.gutter-box-2 {
  background: #99a0e9;
  padding: 5px 0;
}
</style>