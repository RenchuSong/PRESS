import { GPSPoint } from "./gps-point";
import { createMBR, extendMBR, MBR } from "./mbr";

export type GPSTrajectoryData = GPSPoint[];

export interface GPSTrajectoryWithBound {
  data: { lat: number, long: number, t: number }[];
  mbr: MBR;
}

export function createGPSTrajectory(data: GPSTrajectoryData): GPSTrajectoryWithBound {
  if (data.length === 0) {
    return {
      data: [],
      mbr: createMBR({ Lat: 0, Long: 0 })
    }
  } else {
    const mbr = createMBR({ Lat: data[0].LAT, Long: data[0].LONG });
    data.forEach(pt => extendMBR(mbr, { Lat: pt.LAT, Long: pt.LONG }));
    return {
      data: data.map(pt => ({ lat: pt.LAT, long: pt.LONG, t: pt.T })),
      mbr
    }
  }
}
