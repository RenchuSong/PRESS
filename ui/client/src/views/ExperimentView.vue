<template>
  <div>Experiment ID {{ id }}</div>
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";

@Options({
  props: {
    id: String
  },
  components: {}
})
export default class ExperimentView extends Vue {}
</script>
