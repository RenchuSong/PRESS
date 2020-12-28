import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import { key, store } from "./store";

import Button from "ant-design-vue/lib/button";
import DatePicker from "ant-design-vue/lib/date-picker";
import Col from "ant-design-vue/lib/col";
import Row from "ant-design-vue/lib/row";
import Divider from "ant-design-vue/lib/divider";
import Space from "ant-design-vue/lib/space";
import Upload from "ant-design-vue/lib/upload";
import PopConfirm from "ant-design-vue/lib/popconfirm";
import Card from "ant-design-vue/lib/card";

import "ant-design-vue/dist/antd.css";

createApp(App)
  .use(store, key)
  .use(router)
  .use(Button)
  .use(DatePicker)
  .use(Col)
  .use(Row)
  .use(Divider)
  .use(Space)
  .use(Upload)
  .use(PopConfirm)
  .use(Card)
  .mount("#app");
