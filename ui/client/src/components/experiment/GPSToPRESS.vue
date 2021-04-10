<template>
  <a-space direction="vertical" class="full-width">
    <a-page-header
      class="section grid-index"
      title="Map match GPS trajectories and reformat to PRESS trajectories"
    >
      <a-row type="flex" align="middle" justify="space-between" :gutter="10">
        <a-col :span="5">
          <a-select class="full-width" v-model:value="gridIndexBinaryFileName">
            <a-select-option value="tooltip" disabled>
              From Binary
            </a-select-option>
            <!-- <a-select-option
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
            </a-select-option> -->
          </a-select>
        </a-col>
        <a-col :span="5">
          <a-select
            class="full-width"
            v-model:value="roadnetReaderType"
            :disabled="graphReaderChoiceDisabled"
          >
            <a-select-option value="tooltip" disabled>
              With GPS Reader
            </a-select-option>
            <a-select-option
              v-for="roadnetReaderType in roadnetReaderTypes"
              :key="roadnetReaderType"
              :value="roadnetReaderType"
            >
              {{ roadnetReaderType }}
            </a-select-option>
          </a-select>
        </a-col>
        <a-col :span="3">
          <br />
          <a-slider
            :step="0.01"
            v-model:value="sigmaZ"
            :tooltip-visible="true"
            :min="1"
            :max="20"
          />
          σ(z)
        </a-col>
        <a-col :span="4">
          <br />
          <a-slider
            v-model:value="maxGPSBias"
            :tooltip-visible="true"
            :min="5"
            :max="200"
          />
          Max GPS bias (m)
        </a-col>
        <a-col :span="4">
          <br />
          <a-slider
            v-model:value="maxDistDifference"
            :tooltip-visible="true"
            :min="50"
            :max="10000"
          />
          Max dist diff (m)
        </a-col>
        <a-col :span="3">
          <a-button
            class="full-width"
            type="primary"
            @click="preHandleBuildGridIndex"
          >
            START
          </a-button>
        </a-col>
      </a-row>
    </a-page-header>

    <a-page-header
      class="section"
      title="Preview GPS and PRESS trajectory"
      style="height: calc(100vh - 396px)"
    >
      <template #tags>
        <a-tag v-if="trajectories.length > 0" color="green">Ready</a-tag>
        <a-tag v-else color="red">Not ready</a-tag>
      </template>
      <a-row type="flex" justify="space-between" :gutter="10">
        <a-col :span="5">
          <div class="preview-traj-list">
            <a-tree
              showLine
              v-model:expandedKeys="expandedTrajectoryKeys"
              v-model:selectedKeys="selectedTrajectoryKeys"
            >
              <a-tree-node key="trajectories" title="Trajectories">
                <a-tree-node
                  v-for="trajectory in trajectories"
                  :key="trajectory"
                  :title="trajectory"
                  is-leaf
                />
                <a-tree-node
                  key="no-trajectory"
                  title="No Trajectories"
                  v-if="trajectories.length === 0"
                  disabled
                  is-leaf
                />
              </a-tree-node>
            </a-tree>
          </div>
        </a-col>
        <a-col :span="12">
          <div class="gps-and-spatial-preview-container">
            <GeoChart ref="gpsAndSpatialPreview" />
          </div>
        </a-col>
        <a-col :span="7">
          <div class="temporal-preview-container">
            <LineChart xAxis="T" yAxis="D" />
          </div>
        </a-col>
      </a-row>
    </a-page-header>

    <a-row type="flex" justify="end" :gutter="10">
      <a-col style="width: 250px">
        <a-button
          class="full-width"
          type="primary"
          :disabled="trajectories.length === 0"
          @click="gotoGPSToPRESS()"
        >
          Next: TBD
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
  <a-modal
    centered
    :closable="false"
    :maskClosable="false"
    v-model:visible="confirmBuildSPTable"
    title="ATTENTION"
    @ok="
      confirmBuildSPTable = false;
      handleBuildSPTable(spTableRange);
    "
  >
    There is already shortest path table in this experiment.<br />
    Do you want to replace it?
  </a-modal>
</template>

<script lang="ts">
import { computed, defineComponent, onMounted, ref } from "vue";
import { useStore } from "@/store";
import message from "ant-design-vue/lib/message";
import useTrajectories from "@/composables/experiment/useTrajectories";
import useSPTable from "@/composables/experiment/useSPTable";
import useExperiment from "@/composables/experiment/useExperiment";
import { RESTError } from "@/api/base";
import GeoChart from "@/components/charts/GeoChart.vue";
import LineChart from "@/components/charts/LineChart.vue";

export default defineComponent({
  name: "GPSToPRESS",
  setup(_props, _context) {
    const store = useStore();
    const sigmaZ = ref<number>(4.07);
    const maxGPSBias = ref<number>(50);
    const maxDistDifference = ref<number>(2000);

    const gridIndexWidth = ref<number>(200);
    const gridIndexHeight = ref<number>(200);
    const spTableRange = ref<number>(4000);

    const {
      gpsFolderSources,
      trajectories,
      initTrajectories,
      loadTrajectories,
    } = useTrajectories(store);
    const expandedTrajectoryKeys = ref<string[]>(["trajectories"]);
    const selectedTrajectoryKeys = ref<string[]>([]);

    onMounted(async () => {
      await initTrajectories();
    });

    const gridIndexBinaryFileName = ref("tooltip");
    const loadGridIndexFromBinaryDisabled = computed(
      () => gridIndexBinaryFileName.value === "tooltip"
    );

    const confirmBuildGridIndex = ref<boolean>(false);

    const {
      currentSPTableBinaries,
      buildSPTable,
      dumpSPTableToBinary,
      loadSPTableFromBinary,
    } = useSPTable(store);

    const spTableBinaryFileName = ref("tooltip");
    const loadSPTableFromBinaryDisabled = computed(
      () => spTableBinaryFileName.value === "tooltip"
    );

    const confirmBuildSPTable = ref<boolean>(false);

    const handleBuildSPTable = async (distance: number) => {
      try {
        await buildSPTable(distance);
        await dumpSPTableToBinary();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };
    const preHandleBuildSPTable = () => {
      if (currentSPTableBinaries.value.length > 0) {
        confirmBuildSPTable.value = true;
      } else {
        handleBuildSPTable(spTableRange.value);
      }
    };

    const { currentExperimentContext, navigate } = useExperiment(store);

    return {
      sigmaZ,
      maxGPSBias,
      maxDistDifference,

      gpsFolderSources,
      trajectories,
      initTrajectories,
      loadTrajectories,
      expandedTrajectoryKeys,
      selectedTrajectoryKeys,

      loadGridIndexFromBinaryDisabled,
      gridIndexWidth,
      gridIndexHeight,
      gridIndexBinaryFileName,
      confirmBuildGridIndex,
      gridIndexReady: computed(
        () => currentExperimentContext.value?.gridIndexReady
      ),
      currentSPTableBinaries,
      loadSPTableFromBinaryDisabled,
      spTableRange,
      buildSPTable,
      dumpSPTableToBinary,
      spTableBinaryFileName,
      handleLoadSPTableFromBinary: async () => {
        try {
          await loadSPTableFromBinary();
        } catch (exception) {
          message.error((exception as RESTError).message);
        }
      },
      handleBuildSPTable,
      preHandleBuildSPTable,
      confirmBuildSPTable,
      spTableReady: computed(
        () => currentExperimentContext.value?.spTableReady
      ),
      navigate,
    };
  },
  methods: {
    gotoGPSToPRESS() {
      this.navigate(this.$route, this.$router, "gpstopress");
    },
  },
  components: {
    GeoChart,
    LineChart,
  },
});
</script>

<style scoped lang="scss">
.preview-traj-list {
  padding-left: 12px;
  border: 1px solid #ebedf0;
  height: calc(100vh - 480px);
  overflow-y: auto;
}

.gps-and-spatial-preview-container {
  position: absolute;
  top: 0;
  bottom: 0;
  width: calc(100% - 10px);
  overflow: hidden;
}

.temporal-preview-container {
  position: absolute;
  top: 0;
  bottom: 0;
  width: calc(100% - 10px);
  overflow: hidden;
}
</style>
