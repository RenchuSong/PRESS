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

// Get the GPS trajectory.
const std::vector<GPSPoint>& GPSTrajectory::getTrajectory() {
  return trajectory;
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

// Read a PRESS trajectory from files.
PRESSTrajectory::PRESSTrajectory(FileReader& spatialReader, FileReader& temporalReader) {
  // Read spatial component.
  spatialLength = spatialReader.nextInt();
  for (auto i = 0; i < spatialLength; i++) {
    spatialComponent.emplace_back(spatialReader.nextInt());
  }
  // Read temporal component.
  temporalLength = temporalReader.nextInt();
  for (auto i = 0; i < temporalLength; i++) {
    temporalComponent.emplace_back(
      TemporalPair(temporalReader.nextInt(), temporalReader.nextFloat())
    );
  }
}

// Construct a trajectory from spatial and temporal vector.
PRESSTrajectory::PRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal) {
  spatialLength = spatial.size();
  spatialComponent = spatial;
  temporalLength = temporal.size();
  temporalComponent = temporal;
}

// Get spatial component length.
size_t PRESSTrajectory::getSpatialLength() {
  return spatialLength;
}

// Get the spatial component of the PRESS trajectory.
const std::vector<int>& PRESSTrajectory::getSpatialComponent() {
  return spatialComponent;
}

// Get the temporal component length.
size_t PRESSTrajectory::getTemporalLength() {
  return temporalLength;
}

// Get the temporal component of the PRESS trajectory.
const std::vector<TemporalPair>& PRESSTrajectory::getTemporalComponent() {
  return temporalComponent;
}

// Write a PRESS trajectory to the files.
void PRESSTrajectory::store(FileWriter& spatialWriter, FileWriter& temporalWriter) {
  // Write the spatial compoennt to file.
  if (spatialWriter.isBinary()) {
    spatialWriter.writeInt((int)spatialLength);
    for (auto nodeId: spatialComponent) {
      spatialWriter.writeInt(nodeId);
    }
  } else {
    spatialWriter.writeInt((int)spatialLength);
    for (auto nodeId: spatialComponent) {
      spatialWriter.writeChar(' ');
      spatialWriter.writeInt(nodeId);
    }
    spatialWriter.writeChar('\n');
  }
  // Write the temporal component to file.
  if (temporalWriter.isBinary()) {
    temporalWriter.writeInt((int)temporalLength);
    for (auto temporalPair: temporalComponent) {
      temporalWriter.writeInt(temporalPair.t);
      temporalWriter.writeFloat(temporalPair.dist);
    }
  } else {
    temporalWriter.writeInt((int)temporalLength);
    for (auto temporalPair: temporalComponent) {
      temporalWriter.writeChar(' ');
      temporalWriter.writeInt(temporalPair.t);
      temporalWriter.writeChar(' ');
      temporalWriter.writeFloat(temporalPair.dist);
    }
    temporalWriter.writeChar('\n');
  }
}

// Print the PRESS trajectory for debug.
void PRESSTrajectory::print() {
  std::cout << "Spatial: ";
  std::cout << spatialLength << "->";
  for (auto nodeId: spatialComponent) {
    std::cout << " " << nodeId;
  }
  std::cout << std::endl;
  std::cout << "Temporal: ";
  std::cout << temporalLength << "->";
  for (auto temporalPair: temporalComponent) {
    temporalPair.print();
  }
  std::cout << std::endl;
}

PRESSTrajectory::~PRESSTrajectory() { }
