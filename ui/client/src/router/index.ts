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
    path: "/experiment/:id(\\d+)",
    name: "Experiment",
    props: true,
    components: {
      routeHint: () => import("../views/RouteHint.vue"),
      content: () => import("../views/ExperimentView.vue")
    },
    children: [
      {
        path: "",
        component: () => import("../views/experiment/RoadnetView.vue")
      },
      {
        path: "gindexsptable",
        component: () => import("../views/experiment/GridIndexSPTableView.vue")
      },
      {
        path: "gpstopress",
        component: () => import("../views/experiment/GPSToPRESSView.vue")
      },

    ]
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
