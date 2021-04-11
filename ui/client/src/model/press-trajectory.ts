import { DTPair } from "./dt-pair";
import { GeoPath } from "./geo-path";
import { createMBR, extendMBR, MBR } from "./mbr";
import { Shape } from "./shape";

export type PRESSTrajectoryData = {
  spatial: Shape[];
  temporal: DTPair[];
};

export interface PRESSTrajectoryWithBound {
  data: { spatial: { path: GeoPath }[], temporal: DTPair[] };
  mbr: MBR;
}

export function createPRESSTrajectory(data: PRESSTrajectoryData): PRESSTrajectoryWithBound {
  if (data.spatial.length === 0) {
    return {
      data: { spatial: [], temporal: [] },
      mbr: createMBR({ Lat: 0, Long: 0 })
    }
  } else {
    const mbr = createMBR(data.spatial[0][0]);
    data.spatial.forEach(shape => shape.forEach(p => extendMBR(mbr, p)));
    return {
      data: {
        spatial: data.spatial.map(shape => ({ path: shape.map(p => [p.Long, p.Lat]) })),
        temporal: data.temporal
      },
      mbr
    }
  }
}
