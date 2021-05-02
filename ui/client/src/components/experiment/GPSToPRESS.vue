<template>
  <a-space direction="vertical" class="full-width">
    <a-page-header
      class="section"
      title="Map match GPS trajectories and reformat to PRESS trajectories"
    >
      <a-row type="flex" align="middle" justify="space-between" :gutter="10">
        <a-col :span="5">
          <a-select
            class="full-width"
            v-model:value="gpsTrajectoriesFolderName"
            @change="handleChangeGPSFolderName"
          >
            <a-select-option value="tooltip" disabled>
              From Folder
            </a-select-option>
            <a-select-option
              value="noFolder"
              v-if="gpsFolderSources.length === 0"
              disabled
            >
              No Folder Available
            </a-select-option>
            <a-select-option
              v-for="gpsFolderSource in gpsFolderSources"
              :key="gpsFolderSource.sourceName"
              :value="gpsFolderSource.sourceName"
            >
              {{ gpsFolderSource.folderName }}
            </a-select-option>
          </a-select>
        </a-col>
        <a-col :span="5">
          <a-select
            class="full-width"
            v-model:value="gpsReaderType"
            :disabled="gpsReaderChoiceDisabled"
          >
            <a-select-option value="tooltip" disabled>
              With GPS Reader
            </a-select-option>
            <a-select-option
              v-for="gpsReaderType in gpsReaderTypes"
              :key="gpsReaderType"
              :value="gpsReaderType"
            >
              {{ gpsReaderType }}
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
            :disabled="addTrajectoryDisabled"
            @click="preHandleAddTrajectory"
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
        <a-tag v-if="trajectories.length > 1" color="green">Ready</a-tag>
        <a-tag v-else color="red"
          >Not ready (at least 2 trajectories needed)</a-tag
        >
      </template>
      <a-row type="flex" justify="space-between" :gutter="10">
        <a-col :span="5">
          <div class="preview-traj-list">
            <a-tree
              showLine
              v-model:expandedKeys="expandedTrajectoryKeys"
              v-model:selectedKeys="selectedTrajectoryKeys"
              @select="onChooseTrajectory"
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
            <GeoChart
              ref="gpsAndSpatialPreview"
              title="GPS & PRESS spatial component"
            />
          </div>
        </a-col>
        <a-col :span="7">
          <div class="temporal-preview-container">
            <LineChart
              ref="temporalPreview"
              xAxis="T"
              yAxis="D"
              title="PRESS temporal component"
            />
          </div>
        </a-col>
      </a-row>
    </a-page-header>

    <a-row type="flex" justify="end" :gutter="10">
      <a-col style="width: 250px">
        <a-button
          class="full-width"
          type="primary"
          :disabled="trajectories.length < 2"
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
    v-model:visible="confirmAddTrajectories"
    title="ATTENTION"
    @ok="
      confirmAddTrajectories = false;
      handleAddTrajectory(
        sigmaZ,
        maxGPSBias,
        maxDistDifference,
        gpsReaderType,
        gpsFolderSource()
      );
    "
  >
    There are already trajectories in this experiment.<br />
    Do you want to replace them?
  </a-modal>
</template>

<script lang="ts">
import { computed, defineComponent, onMounted, ref } from "vue";
import { useStore } from "@/store";
import message from "ant-design-vue/lib/message";
import useTrajectories from "@/composables/experiment/useTrajectories";
import useExperiment from "@/composables/experiment/useExperiment";
import { RESTError } from "@/api/base";
import GeoChart from "@/components/charts/GeoChart.vue";
import LineChart from "@/components/charts/LineChart.vue";
import { SelectEvent } from "ant-design-vue/lib/tree/Tree";
import moment from "moment";
import { GPSFolderSource } from "@/model/gps-folder-source";

const GPS_TRAJECTORIES_FOLDER = "gps_trajectories";
const MAP_MATCHED_TRAJECTORIES_FOLDER = "map_matched_trajectories";
const PRESS_TRAJECTORIES = "press_trajectories";

export default defineComponent({
  name: "GPSToPRESS",
  setup(_props, _context) {
    const store = useStore();
    const sigmaZ = ref<number>(4.07);
    const maxGPSBias = ref<number>(50);
    const maxDistDifference = ref<number>(2000);

    const {
      gpsReaderTypes,
      gpsFolderSources,
      trajectories,
      loadTrajectories,
      initGPSFolderSources,
      getTrajectory,

      getGPSFilesInFolder,
      clearGPSTrajectoriesInMemory,
      dumpTrajectories,
      reformatTrajectories,
      mapMatch,
    } = useTrajectories(store);
    const expandedTrajectoryKeys = ref<string[]>(["trajectories"]);
    const selectedTrajectoryKeys = ref<string[]>([]);

    onMounted(async () => {
      try {
        await initGPSFolderSources();
        await loadTrajectories();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    });

    const gpsReaderType = ref("tooltip");
    const gpsTrajectoriesFolderName = ref("tooltip");
    const addTrajectoryDisabled = computed(
      () =>
        gpsReaderType.value === "tooltip" ||
        gpsTrajectoriesFolderName.value === "tooltip"
    );
    const gpsReaderChoiceDisabled = computed(
      () =>
        gpsFolderSources.value.find(
          (source) => source.sourceName === gpsTrajectoriesFolderName.value
        )?.gpsReader !== undefined
    );

    const confirmAddTrajectories = ref<boolean>(false);

    const handleAddTrajectory = async (
      sigmaZ: number,
      maxGPSBias: number,
      maxDistDifference: number,
      gpsTrajectoryReaderType: string,
      gpsFolderSource?: GPSFolderSource
    ) => {
      try {
        if (!gpsFolderSource) {
          return;
        }
        const gpsFiles = await getGPSFilesInFolder(gpsFolderSource.sourceName);
        await clearGPSTrajectoriesInMemory();
        for (const gpsFile of gpsFiles) {
          await mapMatch(
            sigmaZ,
            maxGPSBias,
            maxDistDifference,
            gpsTrajectoryReaderType,
            gpsFolderSource.folderName + "/" + gpsFile
          );
        }
        await reformatTrajectories();
        await dumpTrajectories(
          GPS_TRAJECTORIES_FOLDER,
          MAP_MATCHED_TRAJECTORIES_FOLDER,
          PRESS_TRAJECTORIES
        );
        await loadTrajectories();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };
    const preHandleAddTrajectory = () => {
      if (trajectories.value.length > 0) {
        confirmAddTrajectories.value = true;
      } else {
        handleAddTrajectory(
          sigmaZ.value,
          maxGPSBias.value,
          maxDistDifference.value,
          gpsReaderType.value,
          gpsFolderSources.value.find(
            (source) => source.sourceName === gpsTrajectoriesFolderName.value
          )
        );
      }
    };

    const { navigate } = useExperiment(store);

    const handleChangeGPSFolderName = (newGPSFolderName: string) => {
      const dataSource = gpsFolderSources.value.find(
        (source) => source.sourceName === newGPSFolderName
      );
      if (dataSource !== undefined && dataSource.gpsReader !== undefined) {
        gpsReaderType.value = dataSource.gpsReader;
      } else {
        gpsReaderType.value = "tooltip";
      }
    };

    return {
      sigmaZ,
      maxGPSBias,
      maxDistDifference,

      gpsReaderTypes,
      gpsReaderType,
      gpsFolderSources,
      trajectories,
      loadTrajectories,
      expandedTrajectoryKeys,
      selectedTrajectoryKeys,
      addTrajectoryDisabled,
      gpsReaderChoiceDisabled,

      gpsTrajectoriesFolderName,
      confirmAddTrajectories,
      handleAddTrajectory,
      preHandleAddTrajectory,
      navigate,

      getTrajectory,
      handleChangeGPSFolderName,

      gpsFolderSource: () =>
        gpsFolderSources.value.find(
          (source) => source.sourceName === gpsTrajectoriesFolderName.value
        ),
    };
  },
  methods: {
    gotoGPSToPRESS() {
      this.navigate(this.$route, this.$router, "gpstopress");
    },
    async previewTrajectory(id: string) {
      try {
        const trajData = await this.getTrajectory(
          GPS_TRAJECTORIES_FOLDER,
          PRESS_TRAJECTORIES,
          id
        );
        (this.$refs.gpsAndSpatialPreview as any).refreshGPSAndSpatial(
          trajData.gps,
          trajData.press
        );
        (this.$refs.temporalPreview as any).refreshData(
          trajData.press.data.temporal,
          "T",
          "D",
          6,
          (t: number) => moment.unix(t).format("YY/MM/DD HH:mm:ss"),
          (d: number) => `${d} m`
        );
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    },
    async onChooseTrajectory(selectedKeys: string[], info: SelectEvent) {
      if (selectedKeys.length === 1) {
        await this.previewTrajectory(selectedKeys[0]);
      }
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
