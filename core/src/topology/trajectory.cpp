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
  gpsWriter.writeInt((int)length);
  for (auto gpsPoint: trajectory) {
    gpsWriter.writeSeparator();
    gpsWriter.writeInt(gpsPoint.t);
    gpsWriter.writeSeparator();
    gpsWriter.writeFloat(gpsPoint.latitude);
    gpsWriter.writeSeparator();
    gpsWriter.writeFloat(gpsPoint.longitude);
  }
  gpsWriter.writeEol();
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
      TemporalPair(temporalReader.nextFloat(), temporalReader.nextFloat())
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
  spatialWriter.writeInt((int)spatialLength);
  for (auto nodeId: spatialComponent) {
    spatialWriter.writeSeparator();
    spatialWriter.writeInt(nodeId);
  }
  spatialWriter.writeEol();

  // Write the temporal component to file.
  temporalWriter.writeInt((int)temporalLength);
  for (auto temporalPair: temporalComponent) {
    temporalWriter.writeSeparator();
    temporalWriter.writeFloat(temporalPair.t);
    temporalWriter.writeSeparator();
    temporalWriter.writeFloat(temporalPair.dist);
  }
  temporalWriter.writeEol();
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

// Read a compressed PRESS trajectory from files.
PRESSCompressedTrajectory::PRESSCompressedTrajectory(
  FileReader& spatialReader, FileReader& temporalReader
): spatial(Binary(spatialReader)) {
  temporalLength = temporalReader.nextInt();
  for (auto i = 0; i < temporalLength; i++) {
    temporal.emplace_back(
      TemporalPair(temporalReader.nextInt(), temporalReader.nextFloat())
    );
  }
}

// Construct a compressed PRESS trajectory from in-memory objects.
PRESSCompressedTrajectory::PRESSCompressedTrajectory(
  const Binary& spatial, const std::vector<TemporalPair>& temporal
): spatial(spatial) {
  temporalLength = temporal.size();
  this->temporal = temporal;
}

// Write a compressed PRESS trajectory to the files.
void PRESSCompressedTrajectory::store(FileWriter& spatialWriter, FileWriter& temporalWriter) {
  spatial.store(spatialWriter);
  temporalWriter.writeInt((int)temporalLength);
  for (auto tpPair: temporal) {
    temporalWriter.writeSeparator();
    temporalWriter.writeInt(tpPair.t);
    temporalWriter.writeSeparator();
    temporalWriter.writeFloat(tpPair.dist);
  }
  temporalWriter.writeEol();
}

// Get the compressed spatial component.
Binary& PRESSCompressedTrajectory::getSpatialComponent() {
  return spatial;
}

// Get the temporal component length.
size_t PRESSCompressedTrajectory::getTemporalLength() {
  return temporalLength;
}

// Get the compressed temporal component.
const std::vector<TemporalPair>& PRESSCompressedTrajectory::getTemporalComponent() {
  return temporal;
}

// Print the compressed PRESS trajectory for debug.
void PRESSCompressedTrajectory::print() {
  std::cout << "Spatial: ";
  spatial.print();
  std::cout << std::endl;
  std::cout << "Temporal: ";
  std::cout << temporalLength << "->";
  for (auto temporalPair: temporal) {
    temporalPair.print();
  }
  std::cout << std::endl;
}

PRESSCompressedTrajectory::~PRESSCompressedTrajectory() { }
