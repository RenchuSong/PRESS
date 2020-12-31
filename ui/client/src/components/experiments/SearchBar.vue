<template>
  <a-row type="flex" justify="center" :gutter="[16, 16]">
    <a-col :xs="24" :sm="16" :md="10" :lg="8" :xl="6" :xxl="5">
      <div class="experiment-search-wrapper">
        <a-input
          :value="searchQuery"
          @input="$emit('update:searchQuery', $event.target.value)"
          placeholder="Search experiments"
        >
          <template #suffix>
            <search-outlined class="experiment-search-icon" />
          </template>
        </a-input>
      </div>
    </a-col>
    <a-col :xs="24" :sm="6" :md="5" :lg="4" :xl="3" :xxl="2">
      <a-button type="primary" @click="$emit('openCreateExperiment')" block>
        Create
      </a-button>
    </a-col>
  </a-row>
</template>

<script lang="ts">
import { SearchOutlined } from "@ant-design/icons-vue";
import { defineComponent } from "vue";

export default defineComponent({
  name: "SearchBar",
  components: {
    SearchOutlined,
  },
  props: {
    searchQuery: {
      type: String,
      default: "",
    },
  },
});
</script>

<style scoped lang="scss">
.experiment-search-wrapper
  ::v-deep(.ant-input-affix-wrapper-focused)
  .experiment-search-icon {
  color: #108ee9;
}
.experiment-search-wrapper ::v-deep(.experiment-search-icon) {
  color: #6e6e6e;
  transition: all 0.3s cubic-bezier(0.645, 0.045, 0.355, 1);
  font-size: 16px;
}
</style>
