<template>
  <Experiments />
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";
import Experiments from "@/components/Experiments.vue";

@Options({
  components: { Experiments }
})
export default class ExperimentsView extends Vue {}
</script>
