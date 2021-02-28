<template>
  <a-space direction="vertical" class="full-width">
    <a-page-header class="section grid-index" title="Build Grid Index">
      <a-row type="flex" align="middle">
        <a-col style="width: calc(60% - 20px)">
          <a-row type="flex" justify="space-between" :gutter="10">
            <a-col :span="10">
              <br />
              <a-slider
                v-model:value="gridIndexWidth"
                :tooltip-visible="true"
                :min="50"
                :max="1000"
              />
              Cell width (m)
            </a-col>
            <a-col :span="10">
              <br />
              <a-slider
                v-model:value="gridIndexHeight"
                :tooltip-visible="true"
                :min="50"
                :max="1000"
              />
              Cell height (m)
            </a-col>
            <a-col :span="4">
              <a-button
                style="margin-top: 24px"
                class="full-width"
                type="primary"
                @click="preHandleBuildGridIndex"
              >
                BUILD
              </a-button>
            </a-col>
          </a-row>
        </a-col>
        <a-col style="width: 40px">
          <a-row type="flex" justify="center">
            <a-divider type="vertical" style="height: 30px" />
            <span style="color: rgba(0, 0, 0, 0.25)">OR</span>
            <a-divider type="vertical" style="height: 30px" />
          </a-row>
        </a-col>
        <a-col style="width: calc(40% - 20px)">
          <a-row type="flex" justify="space-between" :gutter="10">
            <a-col :span="18">
              <a-select
                class="full-width"
                v-model:value="gridIndexBinaryFileName"
              >
                <a-select-option value="tooltip" disabled>
                  From Binary
                </a-select-option>
                <a-select-option
                  value="noBinary"
                  v-if="currentGridIndexBinaries.length === 0"
                  disabled
                >
                  No Binary Available
                </a-select-option>
                <a-select-option
                  v-for="currentGridIndexBinary in currentGridIndexBinaries"
                  :key="currentGridIndexBinary"
                  :value="currentGridIndexBinary"
                >
                  {{ currentGridIndexBinary }}
                </a-select-option>
              </a-select>
            </a-col>
            <a-col :span="6">
              <a-button
                class="full-width"
                type="primary"
                :disabled="loadFromBinaryDisabled"
                @click="handleLoadGridIndexFromBinary"
              >
                LOAD
              </a-button>
            </a-col>
          </a-row>
        </a-col>
      </a-row>
    </a-page-header>
    <a-row type="flex" justify="end" :gutter="10">
      <a-col style="width: 250px">
        <a-button
          class="full-width"
          type="primary"
          :disabled="!gridIndexReady || !spTableReady"
          @click="gotoGPSTrajToPRESSTraj()"
        >
          Next: GPS Traj. to PRESS Traj.
        </a-button>
      </a-col>
    </a-row>
  </a-space>
  <a-modal
    centered
    :closable="false"
    :maskClosable="false"
    v-model:visible="confirmBuildGridIndex"
    title="ATTENTION"
    @ok="
      confirmBuildGridIndex = false;
      handleBuildGridIndex(gridIndexWidth, gridIndexHeight);
    "
  >
    There is already grid index in this experiment.<br />
    Do you want to replace it?
  </a-modal>
</template>

<script lang="ts">
import { computed, defineComponent, ref } from "vue";
import { useStore } from "@/store";
import message from "ant-design-vue/lib/message";
import useGridIndex from "@/composables/experiment/useGridIndex";
import useExperiment from "@/composables/experiment/useExperiment";
import { RESTError } from "@/api/base";

export default defineComponent({
  name: "GridIndexSPTable",
  setup(_props, _context) {
    const store = useStore();
    const gridIndexWidth = ref<number>(200);
    const gridIndexHeight = ref<number>(200);

    const {
      currentGridIndexBinaries,
      buildGridIndex,
      dumpGridIndexToBinary,
      loadGridIndexFromBinary,
    } = useGridIndex(store);

    const gridIndexBinaryFileName = ref("tooltip");
    const loadFromBinaryDisabled = computed(
      () => gridIndexBinaryFileName.value === "tooltip"
    );

    const confirmBuildGridIndex = ref<boolean>(false);

    const handleBuildGridIndex = async (width: number, height: number) => {
      try {
        await buildGridIndex(width, height);
        await dumpGridIndexToBinary();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };
    const preHandleBuildGridIndex = () => {
      if (currentGridIndexBinaries.value.length > 0) {
        confirmBuildGridIndex.value = true;
      } else {
        handleBuildGridIndex(gridIndexWidth.value, gridIndexHeight.value);
      }
    };
    const { currentExperimentContext, navigate } = useExperiment(store);

    return {
      currentGridIndexBinaries,
      loadFromBinaryDisabled,
      gridIndexWidth,
      gridIndexHeight,
      buildGridIndex,
      dumpGridIndexToBinary,
      gridIndexBinaryFileName,
      handleLoadGridIndexFromBinary: async () => {
        try {
          await loadGridIndexFromBinary();
        } catch (exception) {
          message.error((exception as RESTError).message);
        }
      },
      handleBuildGridIndex,
      preHandleBuildGridIndex,
      confirmBuildGridIndex,
      gridIndexReady: computed(
        () => currentExperimentContext.value?.gridIndexReady
      ),
      spTableReady: true,
      navigate,
    };
  },
  methods: {
    gotoGPSTrajToPRESSTraj() {
      this.navigate(this.$route, this.$router, "gpstopress");
    },
  },
});
</script>

<style scoped lang="scss">
</style>
