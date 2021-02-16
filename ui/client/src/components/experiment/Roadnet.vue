<template>
  <a-space direction="vertical" class="full-width">
    <a-page-header class="section" title="Load Roadnet">
      <a-row type="flex">
        <a-col style="width: calc(60% - 20px)">
          <a-row type="flex" justify="space-between" :gutter="10">
            <a-col :span="10">
              <a-select
                @change="handleChangeRoadnetFileName"
                v-model:value="roadnetFileName"
                class="full-width"
              >
                <a-select-option value="tooltip" disabled>
                  From File
                </a-select-option>
                <a-select-option
                  value="noFile"
                  v-if="roadnetDataSources.length === 0"
                  disabled
                >
                  No Roadnet File Available
                </a-select-option>
                <a-select-option
                  v-for="roadnetDataSource in roadnetDataSources"
                  :key="roadnetDataSource.filename"
                  :value="roadnetDataSource.filename"
                >
                  {{ roadnetDataSource.filename }}
                </a-select-option>
              </a-select>
            </a-col>
            <a-col :span="10">
              <a-select
                class="full-width"
                v-model:value="roadnetReaderType"
                :disabled="graphReaderChoiceDisabled"
              >
                <a-select-option value="tooltip" disabled>
                  With Roadnet Reader
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
            <a-col :span="4">
              <a-button
                class="full-width"
                type="primary"
                :disabled="loadFromFileDisabled"
                @click="preHandleLoadRoadnetFromFile"
              >
                LOAD
              </a-button>
            </a-col>
          </a-row>
        </a-col>
        <a-col style="width: 40px">
          <a-row type="flex" justify="center">
            <a-divider type="vertical" style="height: 5px" />
            <span style="color: rgba(0, 0, 0, 0.25)">OR</span>
            <a-divider type="vertical" style="height: 5px" />
          </a-row>
        </a-col>
        <a-col style="width: calc(40% - 20px)">
          <a-row type="flex" justify="space-between" :gutter="10">
            <a-col :span="18">
              <a-select
                class="full-width"
                v-model:value="roadnetBinaryFileName"
              >
                <a-select-option value="tooltip" disabled>
                  From Binary
                </a-select-option>
                <a-select-option
                  value="noBinary"
                  v-if="currentRoadnetBinaries.length === 0"
                  disabled
                >
                  No Binary Available
                </a-select-option>
                <a-select-option
                  v-for="currentRoadnetBinary in currentRoadnetBinaries"
                  :key="currentRoadnetBinary"
                  :value="currentRoadnetBinary"
                >
                  {{ currentRoadnetBinary }}
                </a-select-option>
              </a-select>
            </a-col>
            <a-col :span="6">
              <a-button
                class="full-width"
                type="primary"
                :disabled="loadFromBinaryDisabled"
                @click="handleLoadRoadnetFromBinary"
              >
                LOAD
              </a-button>
            </a-col>
          </a-row>
        </a-col>
      </a-row>
    </a-page-header>
    <a-page-header
      class="section"
      title="Roadnet Preview"
      style="height: calc(100vh - 348px)"
    >
    </a-page-header>
    <a-row type="flex" justify="end" :gutter="10">
      <a-col style="width: 250px">
        <a-button
          class="full-width"
          type="primary"
          :disabled="!roadnetReady"
          @click="gotoGridIndexAndSPTable()"
        >
          Next: Grid Index & SP Table
        </a-button>
      </a-col>
    </a-row>
  </a-space>
  <a-modal
    centered
    :closable="false"
    :maskClosable="false"
    v-model:visible="confirmLoadRoadnetFromFile"
    title="ATTENTION"
    @ok="
      confirmLoadRoadnetFromFile = false;
      handleLoadRoadnetFromFile(roadnetFileName, roadnetReaderType);
    "
  >
    There are already some binaries in this experiment.<br />
    Loading roadnet from a file will ERASE ALL the binaries.<br />
    Do you want to continue?
  </a-modal>
</template>

<script lang="ts">
import { computed, defineComponent, onMounted, ref } from "vue";
import { useStore } from "@/store";
import useRoadnet from "@/composables/experiment/useRoadnet";
import { RESTError } from "@/api/base";
import message from "ant-design-vue/lib/message";
import useExperiment from "@/composables/experiment/useExperiment";

export default defineComponent({
  name: "Roadnet",
  setup(_props, _context) {
    const store = useStore();
    const {
      initRoadnet,
      loadRoadnetFromFile,
      roadnetReaderTypes,
      roadnetDataSources,
      currentRoadnetBinaries,
      dumpRoadnetToBinary,
      loadRoadnetFromBinary,
    } = useRoadnet(store);
    const { currentExperimentContext, navigate } = useExperiment(store);

    onMounted(async () => {
      await initRoadnet();
    });

    const handleLoadRoadnetFromFile = async (
      filename: string,
      graphReaderType: string
    ) => {
      try {
        await loadRoadnetFromFile(filename, graphReaderType);
        await dumpRoadnetToBinary();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };
    const handleLoadRoadnetFromBinary = async () => {
      try {
        await loadRoadnetFromBinary();
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };

    const roadnetReaderType = ref("tooltip");
    const roadnetFileName = ref("tooltip");
    const roadnetBinaryFileName = ref("tooltip");
    const loadFromFileDisabled = computed(
      () =>
        roadnetReaderType.value === "tooltip" ||
        roadnetFileName.value === "tooltip"
    );
    const graphReaderChoiceDisabled = computed(
      () =>
        roadnetDataSources.value.find(
          (source) => source.filename === roadnetFileName.value
        )?.roadnetReader !== undefined
    );
    const loadFromBinaryDisabled = computed(
      () => roadnetBinaryFileName.value === "tooltip"
    );

    const handleChangeRoadnetFileName = (newRoadnetFilename: string) => {
      const dataSource = roadnetDataSources.value.find(
        (source) => source.filename === newRoadnetFilename
      );
      if (dataSource !== undefined && dataSource.roadnetReader !== undefined) {
        roadnetReaderType.value = dataSource.roadnetReader;
      } else {
        roadnetReaderType.value = "tooltip";
      }
    };

    const confirmLoadRoadnetFromFile = ref<boolean>(false);
    const preHandleLoadRoadnetFromFile = () => {
      if (currentRoadnetBinaries.value.length > 0) {
        confirmLoadRoadnetFromFile.value = true;
      } else {
        handleLoadRoadnetFromFile(
          roadnetFileName.value,
          roadnetReaderType.value
        );
      }
    };

    return {
      handleLoadRoadnetFromFile,
      roadnetReaderTypes,
      roadnetDataSources,
      roadnetReaderType,
      roadnetFileName,
      loadFromFileDisabled,
      graphReaderChoiceDisabled,
      handleChangeRoadnetFileName,
      currentRoadnetBinaries,
      roadnetBinaryFileName,
      loadFromBinaryDisabled,
      roadnetReady: computed(
        () => currentExperimentContext.value?.roadnetReady
      ),
      navigate,
      confirmLoadRoadnetFromFile,
      preHandleLoadRoadnetFromFile,
      handleLoadRoadnetFromBinary,
    };
  },
  methods: {
    gotoGridIndexAndSPTable() {
      this.navigate(this.$route, this.$router, "gindexsptable");
    },
  },
});
</script>

<style scoped lang="scss">
</style>