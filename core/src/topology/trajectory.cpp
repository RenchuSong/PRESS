//
//  trajectory.cpp
//  press-v3
//
//  Created by Renchu Song on 11/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <algorithm>
#include <cassert>
#include <cmath>

#include "trajectory.hpp"

GPSTrajectory::GPSTrajectory() { }

// Read a GPS trajectory from a file.
GPSTrajectory::GPSTrajectory(FileReader& gpsTrajReader) {
  length = gpsTrajReader.nextInt();
  for (auto i = 0; i < length; i++) {
    auto t = gpsTrajReader.nextDouble();
    auto lat = gpsTrajReader.nextDouble();
    auto lng = gpsTrajReader.nextDouble();
    trajectory.emplace_back(GPSPoint(t, lat, lng));
  }
}

// Construct an in-memory GPS trajectory.
GPSTrajectory::GPSTrajectory(const std::vector<GPSPoint>& trajectory) {
  setGPSTrajectory(trajectory);
}

// Construct an in-memory GPS trajectory.
void GPSTrajectory::setGPSTrajectory(const std::vector<GPSPoint>& trajectory) {
  this->trajectory.clear();
  length = trajectory.size();
  this->trajectory.insert(this->trajectory.end(), trajectory.begin(), trajectory.end());
}

// Add a new GPS point to the trajectory.
void GPSTrajectory::addGPSPoint(const GPSPoint& gpsPoint) {
  length++;
  trajectory.emplace_back(gpsPoint);
}

// Get GPS trajectory length.
size_t GPSTrajectory::getLength() const {
  return length;
}

// Get a GPS point by index.
const GPSPoint& GPSTrajectory::getGPSPoint(size_t index) const {
  assert (index < length);
  return trajectory.at(index);
}

// Get the GPS trajectory.
const std::vector<GPSPoint>& GPSTrajectory::getTrajectory() const {
  return trajectory;
}

// Write the GPS trajectory to the file.
void GPSTrajectory::store(FileWriter& gpsWriter) {
  gpsWriter.writeInt((int)length);
  gpsWriter.writeEol();
  for (auto gpsPoint: trajectory) {
    gpsWriter.writeDouble(gpsPoint.t);
    gpsWriter.writeSeparator();
    gpsWriter.writeDouble(gpsPoint.latitude);
    gpsWriter.writeSeparator();
    gpsWriter.writeDouble(gpsPoint.longitude);
    gpsWriter.writeEol();
  }
}

// Print the GPS trajectory for debug.
void GPSTrajectory::print() const {
  std::cout << length << "->";
  for (auto gpsPoint: trajectory) {
    gpsPoint.print();
  }
}

void GPSTrajectory::toJSON(std::stringstream& ss) const {
  ss << "[";
  for (int i = 0; i < length; ++i) {
    if (i > 0) {
      ss << ",";
    }
    ss << trajectory.at(i).toJSONString();
  }
  ss << "]";
}

int GPSTrajectory::getSampleRate() const {
  if (length < 2) {
    return 1;
  }
  std::vector<float> rates;
  for (int i = 1; i < length; ++i) {
    rates.emplace_back(fabs(trajectory.at(i).t - trajectory.at(i - 1).t));
  }
  std::sort(rates.begin(), rates.end());
  return (int)roundf(rates.at((length - 1) / 2));
}

GPSTrajectory::~GPSTrajectory() { }

MapMatchedTrajectory::MapMatchedTrajectory() { }

// Read a map matched trajectory from a file.
MapMatchedTrajectory::MapMatchedTrajectory(FileReader& mmTrajReader) {
  length = mmTrajReader.nextInt();
  for (auto i = 0; i < length; i++) {
    edgeList.emplace_back(mmTrajReader.nextInt());
  }
}

// Construct an in-memory map matched trajectory.
MapMatchedTrajectory::MapMatchedTrajectory(const std::vector<int>& sequence) {
  setMapMatchedTrajectory(sequence);
}

// Set an in-memory map matched trajectory.
void MapMatchedTrajectory::setMapMatchedTrajectory(const std::vector<int>& sequence) {
  length = sequence.size();
  edgeList = sequence;
}

// Get map matched trajectory length.
size_t MapMatchedTrajectory::getLength() const {
  return length;
}

// Get an edge id within the sequence.
int MapMatchedTrajectory::getEdgeId(size_t index) const {
  assert (index < length);
  return edgeList.at(index);
}

// Get the map matched trajectory.
const std::vector<int>& MapMatchedTrajectory::getEdgeList() const {
  return edgeList;
}

// Write the map matched trajectory to the file.
void MapMatchedTrajectory::store(FileWriter& mmTrajWriter) {
  mmTrajWriter.writeInt((int)length);
  for (auto eid: edgeList) {
    mmTrajWriter.writeSeparator();
    mmTrajWriter.writeInt(eid);
  }
  mmTrajWriter.writeEol();
}

// Print the map matched trajectory for debug.
void MapMatchedTrajectory::print() const {
  std::cout << length << "->";
  for (auto eid: edgeList) {
    std::cout << " " << eid;
  }
  std::cout << std::endl;
}

MapMatchedTrajectory::~MapMatchedTrajectory() { }

PRESSTrajectory::PRESSTrajectory() { }

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
      TemporalPair(temporalReader.nextDouble(), temporalReader.nextDouble())
    );
  }
}

// Construct a trajectory from spatial and temporal vector.
PRESSTrajectory::PRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal) {
  setPRESSTrajectory(spatial, temporal);
}

// Set an in-memory PRESS trajectory.
void PRESSTrajectory::setPRESSTrajectory(const std::vector<int> &spatial, const std::vector<TemporalPair> &temporal) {
  spatialLength = spatial.size();
  spatialComponent = spatial;
  temporalLength = temporal.size();
  temporalComponent = temporal;
}

// Get spatial component length.
size_t PRESSTrajectory::getSpatialLength() const {
  return spatialLength;
}

// Get the spatial component of the PRESS trajectory.
const std::vector<int>& PRESSTrajectory::getSpatialComponent() const {
  return spatialComponent;
}

// Get the temporal component length.
size_t PRESSTrajectory::getTemporalLength() const {
  return temporalLength;
}

// Get the temporal component of the PRESS trajectory.
const std::vector<TemporalPair>& PRESSTrajectory::getTemporalComponent() const {
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
    temporalWriter.writeDouble(temporalPair.t);
    temporalWriter.writeSeparator();
    temporalWriter.writeDouble(temporalPair.dist);
  }
  temporalWriter.writeEol();
}

// Print the PRESS trajectory for debug.
void PRESSTrajectory::print() const {
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

void PRESSTrajectory::toJSON(const Graph& graph, std::stringstream& ss) const {
  ss << "{\"spatial\":";
  ss << "[";
  for (int i = 0; i < spatialLength; ++i) {
    if (i > 0) {
      ss << ",";
    }
    graph.getEdge(spatialComponent.at(i)).toJSON(ss);
  }
  ss << "],";
  ss << "\"temporal\":";
  ss << "[";
  for (int i = 0; i < temporalLength; ++i) {
    if (i > 0) {
      ss << ",";
    }
    ss << temporalComponent.at(i).toJSONString();
  }
  ss << "]";
  ss << "}";
}

PRESSTrajectory::~PRESSTrajectory() { }

// Read a compressed PRESS trajectory from files.
PRESSCompressedTrajectory::PRESSCompressedTrajectory(
  FileReader& spatialReader, FileReader& temporalReader
): spatial(Binary(spatialReader)) {
  temporalLength = temporalReader.nextInt();
  for (auto i = 0; i < temporalLength; i++) {
    temporal.emplace_back(
      TemporalPair(temporalReader.nextDouble(), temporalReader.nextDouble())
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
    temporalWriter.writeDouble(tpPair.t);
    temporalWriter.writeSeparator();
    temporalWriter.writeDouble(tpPair.dist);
  }
  temporalWriter.writeEol();
}

// Get the compressed spatial component.
const Binary& PRESSCompressedTrajectory::getSpatialComponent() const {
  return spatial;
}

// Get the temporal component length.
size_t PRESSCompressedTrajectory::getTemporalLength() const {
  return temporalLength;
}

// Get the compressed temporal component.
const std::vector<TemporalPair>& PRESSCompressedTrajectory::getTemporalComponent() const {
  return temporal;
}

// Print the compressed PRESS trajectory for debug.
void PRESSCompressedTrajectory::print() const {
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
