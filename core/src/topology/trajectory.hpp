//
//  trajectory.hpp
//  press-v3
//
//  Created by Renchu Song on 11/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef trajectory_hpp
#define trajectory_hpp

#include <sstream>
#include <vector>

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "../io/binary.hpp"
#include "./graph.hpp"
#include "geospatial_structures.hpp"

// GPS trajectory.
class GPSTrajectory {
private:
  size_t length;
  std::vector<GPSPoint> trajectory;

public:
  GPSTrajectory();
  // Read a GPS trajectory from a file.
  GPSTrajectory(FileReader& gpsTrajReader);
  // Construct an in-memory GPS trajectory.
  GPSTrajectory(const std::vector<GPSPoint>& sequence);
  // Set an in-memory GPS trajectory.
  void setGPSTrajectory(const std::vector<GPSPoint>& sequence);
  // Add a new GPS point to the trajectory.
  void addGPSPoint(const GPSPoint& gpsPoint);
  size_t getLength() const;
  const GPSPoint& getGPSPoint(size_t index) const;
  const std::vector<GPSPoint>& getTrajectory() const;
  void store(FileWriter& gpsWriter);
  void print() const;
  void toJSON(std::stringstream& ss) const;
  int getSampleRate() const;
  ~GPSTrajectory();
};

// Map matched trajectory.
class MapMatchedTrajectory {
private:
  size_t length;
  std::vector<int> edgeList;
public:
  MapMatchedTrajectory();
  // Read a map matched trajectory from a file.
  MapMatchedTrajectory(FileReader& mmTrajReader);
  // Construct an in-memory map matched trajectory.
  MapMatchedTrajectory(const std::vector<int>& sequence);
  // Set an in-memeory map matched trajectory.
  void setMapMatchedTrajectory(const std::vector<int>& sequence);
  size_t getLength() const;
  int getEdgeId(size_t index) const;
  const std::vector<int>& getEdgeList() const;
  void store(FileWriter& mmTrajWriter);
  void print() const;
  ~MapMatchedTrajectory();
};

// Raw PRESS trajectory.
class PRESSTrajectory {
private:
  size_t spatialLength;
  size_t temporalLength;
  // Spatial component is represented by a list of edge ids in the graph.
  std::vector<int> spatialComponent;
  // Temporal component is represented by a list of temporal pairs.
  std::vector<TemporalPair> temporalComponent;
  
public:
  PRESSTrajectory();
  // Read a PRESS trajectory from files.
  PRESSTrajectory(FileReader& spatialReader, FileReader& temporalReader);
  // Construct an in-memory PRESS trajectory.
  PRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal);
  // Set an in-memory PRESS trajectory.
  void setPRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal);
  void store(FileWriter& spatialWriter, FileWriter& temporalWriter);
  size_t getSpatialLength() const;
  const std::vector<int>& getSpatialComponent() const;
  size_t getTemporalLength() const;
  const std::vector<TemporalPair>& getTemporalComponent() const;
  void print() const;
  void toJSON(const Graph& graph, std::stringstream& ss) const;
  ~PRESSTrajectory();
};

// Compressed PRESS trajectory.
class PRESSCompressedTrajectory {
private:
  Binary spatial;
  size_t temporalLength;
  std::vector<TemporalPair> temporal;

public:
  // Read a compressed PRESS trajectory from files.
  PRESSCompressedTrajectory(FileReader& spatialReader, FileReader& temporalReader);
  // Construct an in-memory compressed PRESS trajectory.
  PRESSCompressedTrajectory(const Binary& spatial, const std::vector<TemporalPair>& temporal);
  void store(FileWriter& spatialWriter, FileWriter& temporalWriter);
  const Binary& getSpatialComponent() const;
  size_t getTemporalLength() const;
  const std::vector<TemporalPair>& getTemporalComponent() const;
  void print() const;
  ~PRESSCompressedTrajectory();
};

#endif /* trajectory_hpp */
