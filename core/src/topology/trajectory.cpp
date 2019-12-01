//
//  trajectory.cpp
//  press-v3
//
//  Created by Renchu Song on 11/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>

#include "trajectory.hpp"

// Read a GPS trajectory from a file.
GPSTrajectory::GPSTrajectory(FileReader& gpsTrajReader) {
  length = gpsTrajReader.nextInt();
  for (auto i = 0; i < length; i++) {
    auto t = gpsTrajReader.nextInt();
    auto lat = gpsTrajReader.nextFloat();
    auto lng = gpsTrajReader.nextFloat();
    trajectory.emplace_back(GPSPoint(t, lat, lng));
  }
}

// Construct an in-memory GPS trajectory.
GPSTrajectory::GPSTrajectory(std::vector<GPSPoint>& trajectory) {
  length = trajectory.size();
  this->trajectory = trajectory;
}

// Get GPS trajectory length.
size_t GPSTrajectory::getLength() {
  return length;
}

// Get a GPS point by index.
GPSPoint& GPSTrajectory::getGPSPoint(size_t index) {
  assert (index < length);
  return trajectory[index];
}

// Write the GPS trajectory to the file.
void GPSTrajectory::store(FileWriter& gpsWriter) {
  if (gpsWriter.isBinary()) {
    // Writing to a binary file.
    gpsWriter.writeInt((int)length);
    for (auto gpsPoint: trajectory) {
      gpsWriter.writeInt(gpsPoint.t);
      gpsWriter.writeFloat(gpsPoint.latitude);
      gpsWriter.writeFloat(gpsPoint.longitude);
    }
  } else {
    // Writing to a normal file.
    gpsWriter.writeInt((int)length);
    for (auto gpsPoint: trajectory) {
      gpsWriter.writeChar(' ');
      gpsWriter.writeInt(gpsPoint.t);
      gpsWriter.writeChar(' ');
      gpsWriter.writeFloat(gpsPoint.latitude);
      gpsWriter.writeChar(' ');
      gpsWriter.writeFloat(gpsPoint.longitude);
    }
    gpsWriter.writeChar('\n');
  }
}

// Print the GPS trajectory for debug.
void GPSTrajectory::print() {
  std::cout << length << "->";
  for (auto gpsPoint: trajectory) {
    gpsPoint.print();
  }
}

GPSTrajectory::~GPSTrajectory() { }

