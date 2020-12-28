<template>
  <Experiment :id="id" />
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import Experiment from "@/components/Experiment.vue";

@Options({
  props: {
    id: String,
  },
  components: { Experiment },
})
export default class ExperimentView extends Vue {}
</script>
