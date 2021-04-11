import { Position } from "./position";

export interface MBR {
  minPos: Position;
  maxPos: Position;
}

export function createMBR(pos: Position): MBR {
  return {
    minPos: {
      Lat: pos.Lat,
      Long: pos.Long
    },
    maxPos: {
      Lat: pos.Lat,
      Long: pos.Long
    }
  };
}

export function extendMBR(mbr: MBR, pos: Position) {
  mbr.minPos.Lat = Math.min(mbr.minPos.Lat, pos.Lat);
  mbr.minPos.Long = Math.min(mbr.minPos.Long, pos.Long);
  mbr.maxPos.Lat = Math.max(mbr.maxPos.Lat, pos.Lat);
  mbr.maxPos.Long = Math.max(mbr.maxPos.Long, pos.Long);
}

export function mergeMBR(mbr1: MBR, mbr2: MBR): MBR {
  const mbr = createMBR(mbr1.minPos);
  extendMBR(mbr, mbr1.maxPos);
  extendMBR(mbr, mbr2.minPos);
  extendMBR(mbr, mbr2.maxPos);
  return mbr;
}

export function center(mbr: MBR): Position {
  return {
    Lat: (mbr.minPos.Lat + mbr.maxPos.Lat) / 2,
    Long: (mbr.minPos.Long + mbr.maxPos.Long) / 2
  };
}
