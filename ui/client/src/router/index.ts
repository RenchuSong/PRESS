import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/",
    name: "Experiments",
    component: () => import("../views/ExperimentsView.vue")
  },
  {
    path: "/experiment/:id",
    name: "Experiment",
    props: true,
    component: () => import("../views/ExperimentView.vue")
  },
  {
    path: "/:pathMatch(.*)*",
    name: "NotFound",
    component: () => import("../views/NotFound.vue")
  }
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
});

export default router;
