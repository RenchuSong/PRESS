<template>
  <div>
    <div id="line"></div>
  </div>
</template>
<script lang="ts">
import { defineComponent, onMounted } from "vue";
import { Chart } from "@antv/g2";

export default defineComponent({
  name: "LineChartComponent",
  props: {
    xAxis: String,
    yAxis: String,
  },
  setup(props, _context) {
    let chart: Chart | undefined = undefined;

    const refreshData = (
      rawData: any[],
      x: string,
      y: string,
      xTickCount = 6,
      xFormatter?: () => {},
      yFormatter?: () => {}
    ) => {
      chart?.data(rawData);
      chart?.scale({
        [x]: {
          tickCount: xTickCount,
        },
        [y]: {
          nice: true,
        },
      });
      if (xFormatter) {
        chart?.axis(x, {
          label: {
            formatter: xFormatter,
          },
        });
      }
      if (yFormatter) {
        chart?.axis(y, {
          label: {
            formatter: yFormatter,
          },
        });
      }

      chart?.line().position(`${x}*${y}`).color("#1890ff");
      chart?.render();
    };

    onMounted(async () => {
      chart = new Chart({
        container: "line",
        autoFit: true,
      });
      refreshData(
        [
          {
            [props.xAxis || "x"]: 0,
            [props.yAxis || "y"]: 0,
          },
          {
            [props.xAxis || "x"]: 1,
            [props.yAxis || "y"]: 1,
          },
        ],
        props.xAxis || "x",
        props.yAxis || "y"
      );
    });

    return {
      chart,
      refreshData,
    };
  },
});
</script>
<style>
#line {
  position: absolute;
  top: 0;
  bottom: 0;
  width: 100%;
}
</style>
