import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import { key, store } from "./store";
import DatePicker from 'ant-design-vue/lib/date-picker';
import "ant-design-vue/dist/antd.css"

createApp(App)
  .use(store, key)
  .use(router)
  .use(DatePicker)
  .mount("#app");
