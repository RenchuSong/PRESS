//
//  gps_trajectory_reader_base.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "gps_trajectory_reader_base.hpp"

#include <cassert>

#include "../../util/helper.hpp"

void GPSTrajectoryReaderBase::readGPSTrajectory(
  const std::string& fileName,
  GPSTrajectory& gpsTrajectory
) {
  readGPSTrajectoryImpl(fileName, gpsTrajectory);
  sanityCheck(gpsTrajectory);
}

void GPSTrajectoryReaderBase::sanityCheck(GPSTrajectory& gpsTrajectory) const {
  // 1. Trajectory cannot be empty.
  assert(gpsTrajectory.getLength() > 0);

  // 2. All latitudes and longitudes fall in reasonable range.
  GPSPoint minLatGPS(-90, 0);
  auto minBoundY = gps2Point2D(minLatGPS);
  GPSPoint minLongGPS(0, -180);
  auto minBoundX = gps2Point2D(minLongGPS);
  GPSPoint maxLatGPS(90, 0);
  auto maxBoundY = gps2Point2D(maxLatGPS);
  GPSPoint maxLongGPS(0, 180);
  auto maxBoundX = gps2Point2D(maxLongGPS);
  Point2D minBound(minBoundX.x, minBoundY.y);
  Point2D maxBound(maxBoundX.x, maxBoundY.y);
  for (auto& gpsPoint: gpsTrajectory.getTrajectory()) {
    assert(pointInMBR(gps2Point2D(gpsPoint), minBound, maxBound));
  }
}
