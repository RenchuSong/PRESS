//
//  trajectory.hpp
//  press-v3
//
//  Created by Renchu Song on 11/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef trajectory_hpp
#define trajectory_hpp

#include <vector>

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "../io/binary.hpp"
#include "auxiliaries.hpp"

// GPS trajectory.
class GPSTrajectory {
private:
  size_t length;
  std::vector<GPSPoint> trajectory;

public:
  // Read a GPS trajectory from a file.
  GPSTrajectory(FileReader& gpsTrajReader);
  // Construct an in-memory GPS trajectory.
  GPSTrajectory(std::vector<GPSPoint>& sequence);
  size_t getLength();
  GPSPoint& getGPSPoint(size_t index);
  const std::vector<GPSPoint>& getTrajectory();
  void store(FileWriter& gpsWriter);
  void print();
  ~GPSTrajectory();
};

// Map matched trajectory.
class MapMatchedTrajectory {
private:
  size_t length;
  std::vector<int> edgeList;
public:
  // Read a map matched trajectory from a file.
  MapMatchedTrajectory(FileReader& mmTrajReader);
  // Construct an in-memory map matched trajectory.
  MapMatchedTrajectory(std::vector<int>& sequence);
  size_t getLength();
  int getEdgeId(size_t index);
  const std::vector<int>& getEdgeList();
  void store(FileWriter& mmTrajWriter);
  void print();
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
  // Read a PRESS trajectory from files.
  PRESSTrajectory(FileReader& spatialReader, FileReader& temporalReader);
  // Construct an in-memory PRESS trajectory.
  PRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal);
  void store(FileWriter& spatialWriter, FileWriter& temporalWriter);
  size_t getSpatialLength();
  const std::vector<int>& getSpatialComponent();
  size_t getTemporalLength();
  const std::vector<TemporalPair>& getTemporalComponent();
  void print();
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
  Binary& getSpatialComponent();
  size_t getTemporalLength();
  const std::vector<TemporalPair>& getTemporalComponent();
  void print();
  ~PRESSCompressedTrajectory();
};

#endif /* trajectory_hpp */
