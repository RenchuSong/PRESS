import { createMBR, extendMBR, MBR } from "./mbr";
import { Shape } from "./shape";

export type RoadnetData = Shape[];

export interface RoadnetWithBound {
  data: RoadnetData;
  mbr: MBR;
}

export function createRoadnet(data: RoadnetData): RoadnetWithBound {
  if (data.length === 0) {
    return {
      data,
      mbr: createMBR({ Lat: 0, Long: 0 })
    }
  } else {
    const mbr = createMBR(data[0][0]);
    data.forEach(shape => shape.forEach(p => extendMBR(mbr, p)));
    return {
      data,
      mbr
    }
  }
}
