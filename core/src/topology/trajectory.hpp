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
  void store(FileWriter& gpsWriter);
  void print();
  ~GPSTrajectory();
};

class PRESSTrajectory {
private:
  size_t spatialLength;
  size_t temporalLength;
  // Spatial component is represented by a list of node ids in the graph.
  std::vector<int> spatialComponent;
  // Temporal component is represented by a list of temporal pairs.
  std::vector<TemporalPair> temporalComponent;
  
public:
  // Read a PRESS trajectory from files.
  PRESSTrajectory(const FileReader& spatialReader, const FileReader& temporalReader);
  // Construct an in-memory PRESS trajectory.
  PRESSTrajectory(const std::vector<int>& spatial, const std::vector<TemporalPair>& temporal);
  void store(const FileWriter& spatialWriter, const FileWriter& temporalWriter);
  void print();
  ~PRESSTrajectory();
};

#endif /* trajectory_hpp */
