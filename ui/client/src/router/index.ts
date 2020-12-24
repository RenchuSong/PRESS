import { createRouter, createWebHistory, RouteRecordRaw } from "vue-router";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/",
    name: "Experiments",
    component: () =>
      import(/* webpackChunkName: "about" */ "../views/Experiments.vue")
  },
  {
    path: "/experiment/:id",
    name: "Experiment",
    component: () =>
      import(/* webpackChunkName: "about" */ "../views/Experiment.vue")
  },
];

const router = createRouter({
  history: createWebHistory(process.env.BASE_URL),
  routes
});

export default router;
