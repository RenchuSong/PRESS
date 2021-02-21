<template>
  <div>
    <div id="map"></div>
  </div>
</template>
<script lang="ts">
import { defineComponent, onMounted } from "vue";
import { Scene, PointLayer, LineLayer, ILayer } from "@antv/l7";
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
    let roadnetLayer: ILayer;

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
      scene?.setCenter([position.Long, position.Lat]);
    };

    const setZoom = (zoom: number) => {
      scene?.setZoom(zoom);
    };

    const refreshRoadnet = (roadnet: RoadnetWithBound) => {
      setCenter(center(roadnet.mbr));
      if (roadnetLayer !== undefined) {
        scene?.removeLayer(roadnetLayer);
      }
      roadnetLayer = new LineLayer({})
        .source(roadnet.data, {
          parser: {
            coordinates: "path",
            type: "json",
          },
        })
        .size(0.5)
        .shape("line")
        .color("#1890ff");
      scene?.addLayer(roadnetLayer);
      scene?.fitBounds([
        [roadnet.mbr.minPos.Long, roadnet.mbr.minPos.Lat],
        [roadnet.mbr.maxPos.Long, roadnet.mbr.maxPos.Lat],
      ]);
    };

    return {
      scene,
      setCenter,
      setZoom,
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
