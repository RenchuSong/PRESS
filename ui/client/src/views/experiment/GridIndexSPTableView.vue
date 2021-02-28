<template>
  <GridIndexSPTable />
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import GridIndexSPTable from "@/components/experiment/GridIndexSPTable.vue";

@Options({
  components: { GridIndexSPTable },
})
export default class GridIndexSPTableView extends Vue {}
</script>

<style lang="scss">
.ant-tooltip {
  z-index: 0 !important;
}
</style>
