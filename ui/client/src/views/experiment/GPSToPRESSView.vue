<template>
  <GPSToPRESS />
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import GPSToPRESS from "@/components/experiment/GPSToPRESS.vue";

@Options({
  components: { GPSToPRESS },
})
export default class GPSToPRESSView extends Vue {}
</script>

<style lang="scss">
.ant-tooltip {
  z-index: 0 !important;
}
</style>
