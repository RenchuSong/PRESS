<template>
  <div>Experiment ID {{ $route.params.id }}</div>
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";

@Options({
  components: {},
})
export default class Experiment extends Vue {}
</script>
