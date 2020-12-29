<template>
  <a-breadcrumb class="route-hint" :routes="routes">
    <template #itemRender="{ route, routes, paths }">
      <!-- {{ route }} -->
      <span v-if="routes.indexOf(route) === routes.length - 1">
        {{ route.breadcrumbName }}
      </span>
      <router-link v-else :to="`${basePath}/${paths.join('/')}`">
        {{ route.breadcrumbName }}
      </router-link>
    </template>
  </a-breadcrumb>
</template>

<script lang="ts">
import { defineComponent } from "vue";

export default defineComponent({
  name: "RouteHint",
  props: {
    id: String,
  },
  setup(props, context) {
    return {
      props,
      basePath: "",
      routes: [
        {
          path: "",
          breadcrumbName: "Experiments",
        },
        {
          path: "second",
          breadcrumbName: "second",
        },
        {
          path: "first",
          breadcrumbName: "first",
          children: [
            {
              path: "/general",
              breadcrumbName: "General",
            },
            {
              path: "/layout",
              breadcrumbName: "Layout",
            },
            {
              path: "/navigation",
              breadcrumbName: "Navigation",
            },
          ],
        },
      ],
    };
  },
});
</script>
