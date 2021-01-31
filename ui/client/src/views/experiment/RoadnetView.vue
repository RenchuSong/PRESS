<template>
  <Roadnet />
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import Roadnet from "@/components/experiment/Roadnet.vue";

@Options({
  components: { Roadnet },
})
export default class RoadnetView extends Vue {}
</script>
