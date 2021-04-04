import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import { key, store } from "./store";

import Empty from "ant-design-vue/lib/empty";
import Spin from "ant-design-vue/lib/spin";
import Button from "ant-design-vue/lib/button";
import DatePicker from "ant-design-vue/lib/date-picker";
import Col from "ant-design-vue/lib/col";
import Row from "ant-design-vue/lib/row";
import Divider from "ant-design-vue/lib/divider";
import Space from "ant-design-vue/lib/space";
import Upload from "ant-design-vue/lib/upload";
import PopConfirm from "ant-design-vue/lib/popconfirm";
import Card from "ant-design-vue/lib/card";
import Layout from "ant-design-vue/lib/layout";
import Menu from "ant-design-vue/lib/menu";
import Breadcrumb from "ant-design-vue/lib/breadcrumb";
import Tooltip from "ant-design-vue/lib/tooltip";
import Input from "ant-design-vue/lib/input";
import Modal from "ant-design-vue/lib/modal";
import Form from "ant-design-vue/lib/form";
import Steps from "ant-design-vue/lib/steps";
import PageHeader from "ant-design-vue/lib/page-header";
import Descriptions from "ant-design-vue/lib/descriptions";
import Select from "ant-design-vue/lib/select";
import Slider from "ant-design-vue/lib/slider";
import Tag from "ant-design-vue/lib/tag";
import Tree from "ant-design-vue/lib/tree";

import "ant-design-vue/dist/antd.css";

createApp(App)
  .use(store, key)
  .use(router)
  .use(Empty)
  .use(Button)
  .use(DatePicker)
  .use(Col)
  .use(Row)
  .use(Divider)
  .use(Space)
  .use(Upload)
  .use(PopConfirm)
  .use(Card)
  .use(Layout)
  .use(Menu)
  .use(Breadcrumb)
  .use(Tooltip)
  .use(Input)
  .use(Modal)
  .use(Form)
  .use(Spin)
  .use(Steps)
  .use(PageHeader)
  .use(Descriptions)
  .use(Select)
  .use(Slider)
  .use(Tag)
  .use(Tree)
  .mount("#app");
