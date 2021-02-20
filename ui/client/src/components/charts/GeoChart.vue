<template>
  <div>
    <div id="map"></div>
  </div>
</template>
<script lang="ts">
import { defineComponent, onMounted } from "vue";
import { Scene, PointLayer } from "@antv/l7";
import { Mapbox } from "@antv/l7-maps";
import { RoadnetWithBound } from "@/model/roadnet";
import { Position } from "@/model/position";
import { center } from "@/model/mbr";
export default defineComponent({
  name: "GeoChartComponent",
  props: {
    id: String,
  },
  setup(props, _context) {
    let scene: Scene | undefined = undefined;
    onMounted(async () => {
      scene = new Scene({
        id: "map",
        map: new Mapbox({
          pitch: 0,
          style: "light",
          center: [180, 0],
          zoom: 0,
          minZoom: 0,
        }),
      });
    });

    const setCenter = (position: Position) => {
      if (scene !== undefined) {
        scene.setCenter([position.Long, position.Lat]);
      }
    };

    const refreshRoadnet = (roadnet: RoadnetWithBound) => {
      setCenter(center(roadnet.mbr));
    };

    return {
      scene,
      setCenter,
      refreshRoadnet,
    };
  },
});
</script>
<style>
::-webkit-scrollbar {
  display: none;
}

html,
body {
  overflow: hidden;
  margin: 0;
}

#map {
  position: absolute;
  top: 0;
  bottom: 0;
  width: 100%;
}
</style>
