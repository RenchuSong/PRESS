<template>
  <a-layout class="top-level-content-layout">
    <a-layout-content :style="{ padding: '50px 0', minHeight: '280px' }">
      <a-empty
        :image="imageUrl"
        :image-style="{
          height: '200px',
        }"
      >
        <template #description></template>
        <router-link to="/">
          <a-button type="primary"> All experiments </a-button>
        </router-link>
      </a-empty>
    </a-layout-content>
  </a-layout>
</template>

<script lang="ts">
import { Options, Vue } from "vue-class-component";

@Options({
  components: {},
  computed: {
    imageUrl() {
      return require("@/assets/404.png");
    },
  },
})
export default class NotFound extends Vue {}
</script>
