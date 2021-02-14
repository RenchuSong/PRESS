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
                @change="handleChange"
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
                @click="
                  handleLoadRoadnetFromFile(roadnetFileName, roadnetReaderType)
                "
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
              <a-select @change="handleChange" class="full-width">
                <a-select-option value="tooltip" disabled>
                  From Binary
                </a-select-option>
                <a-select-option value="lucy">Lucy</a-select-option>
                <a-select-option value="Yiminghe">yiminghe</a-select-option>
              </a-select>
            </a-col>
            <a-col :span="6">
              <a-button
                class="full-width"
                type="primary"
                @click="
                  handleLoadRoadnetFromFile(
                    'WA_roadnetwork_and_single_trajectory',
                    'SEATTLE_SAMPLE_ROADNET'
                  )
                "
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
          @click="
            handleLoadRoadnetFromFile(
              'WA_roadnetwork_and_single_trajectory',
              'SEATTLE_SAMPLE_ROADNET'
            )
          "
        >
          Next: Grid Index & SP Table
        </a-button>
      </a-col>
    </a-row>
  </a-space>
</template>

<script lang="ts">
import { computed, defineComponent, onMounted, ref } from "vue";
import { useStore } from "@/store";
import useRoadnet from "@/composables/experiment/useRoadnet";
import { RESTError } from "@/api/base";
import message from "ant-design-vue/lib/message";

export default defineComponent({
  name: "Roadnet",
  setup(_props, _context) {
    const store = useStore();
    const {
      initRoadnet,
      loadRoadnetFromFile,
      roadnetReaderTypes,
      roadnetDataSources,
    } = useRoadnet(store);

    onMounted(async () => {
      await initRoadnet();
    });

    const handleLoadRoadnetFromFile = async (
      filename: string,
      graphReaderType: string
    ) => {
      try {
        await loadRoadnetFromFile(filename, graphReaderType);
      } catch (exception) {
        message.error((exception as RESTError).message);
      }
    };

    const roadnetReaderType = ref("tooltip");
    const roadnetFileName = ref("tooltip");
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

    return {
      handleLoadRoadnetFromFile,
      roadnetReaderTypes,
      roadnetDataSources,
      roadnetReaderType,
      roadnetFileName,
      loadFromFileDisabled,
      graphReaderChoiceDisabled,
      handleChangeRoadnetFileName,
    };
  },
});
</script>

<style scoped lang="scss">
</style>
