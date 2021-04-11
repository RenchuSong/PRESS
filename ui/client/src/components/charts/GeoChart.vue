<template>
  <div>
    <div id="map"></div>
    <div class="title">{{ title }}</div>
  </div>
</template>
<script lang="ts">
import { defineComponent, onMounted } from "vue";
import { Scene, LineLayer, ILayer, PointLayer } from "@antv/l7";
import { Mapbox } from "@antv/l7-maps";
import { RoadnetWithBound } from "@/model/roadnet";
import { Position } from "@/model/position";
import { center, mergeMBR } from "@/model/mbr";
import { GPSTrajectoryWithBound } from "@/model/gps-trajectory";
import { PRESSTrajectoryWithBound } from "@/model/press-trajectory";
export default defineComponent({
  name: "GeoChartComponent",
  props: {
    id: String,
    title: String,
  },
  setup(props, _context) {
    let scene: Scene | undefined = undefined;
    let roadnetLayer: ILayer;
    let gpsLayer: ILayer;
    let pressSpatialLayer: ILayer;

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

    const refreshGPSAndSpatial = (
      gps: GPSTrajectoryWithBound,
      press: PRESSTrajectoryWithBound
    ) => {
      const mbr = mergeMBR(gps.mbr, press.mbr);
      setCenter(center(mbr));

      // PRESS spatial layer
      if (pressSpatialLayer !== undefined) {
        scene?.removeLayer(pressSpatialLayer);
      }
      pressSpatialLayer = new LineLayer({})
        .source(press.data.spatial, {
          parser: {
            coordinates: "path",
            type: "json",
          },
        })
        .size(1)
        .shape("line")
        .color("#52c41a");
      scene?.addLayer(pressSpatialLayer);

      // GPS layer
      if (gpsLayer !== undefined) {
        scene?.removeLayer(gpsLayer);
      }
      gpsLayer = new PointLayer({})
        .source(gps.data, {
          parser: {
            type: "json",
            x: "long",
            y: "lat",
          },
        })
        .shape("circle")
        .size(2)
        .color("#1890ff");
      scene?.addLayer(gpsLayer);

      scene?.fitBounds([
        [mbr.minPos.Long, mbr.minPos.Lat],
        [mbr.maxPos.Long, mbr.maxPos.Lat],
      ]);
    };

    return {
      scene,
      setCenter,
      setZoom,
      refreshRoadnet,
      refreshGPSAndSpatial,
    };
  },
});
</script>
<style scoped lang="scss">
#map {
  position: absolute;
  top: 0;
  bottom: 0;
  width: 100%;
}

.title {
  position: absolute;
  top: 4px;
  width: 100%;
  text-align: center;
  font-size: 18px;
}
</style>
