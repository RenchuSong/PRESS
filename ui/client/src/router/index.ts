import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/",
    name: "Experiments",
    props: true,
    components: {
      routeHint: () => import("../views/RouteHint.vue"),
      content: () => import("../views/ExperimentsView.vue")
    }
  },
  {
    path: "/experiment/:id",
    name: "Experiment",
    props: true,
    components: {
      routeHint: () => import("../views/RouteHint.vue"),
      content: () => import("../views/ExperimentView.vue")
    }
  },
  {
    path: "/:pathMatch(.*)*",
    name: "NotFound",
    props: true,
    components: {
      routeHint: () => import("../views/RouteHint.vue"),
      content: () => import("../views/NotFound.vue")
    }
  }
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
});

export default router;
