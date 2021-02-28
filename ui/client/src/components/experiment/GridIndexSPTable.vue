<template>
  <a-space direction="vertical" class="full-width">
    <a-page-header class="section" title="Build Grid Index">
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
                :disabled="loadFromFileDisabled"
                @click="preHandleLoadRoadnetFromFile"
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
                v-model:value="roadnetBinaryFileName"
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
                @click="handleLoadRoadnetFromBinary"
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
          @click="gotoGridIndexAndSPTable()"
        >
          Next: GPS Traj. to PRESS Traj.
        </a-button>
      </a-col>
    </a-row>
  </a-space>
</template>

<script lang="ts">
import { defineComponent, ref } from "vue";
import { useStore } from "@/store";

export default defineComponent({
  name: "GridIndexSPTable",
  setup(_props, _context) {
    const store = useStore();
    return {
      currentGridIndexBinaries: [],
      loadFromBinaryDisabled: false,
      gridIndexWidth: ref<number>(200),
      gridIndexHeight: ref<number>(200),
    };
  },
  methods: {},
});
</script>

<style scoped lang="scss">
</style>
