//
//  factory.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef factory_hpp
#define factory_hpp

#include <memory>

#include "./gps_trajectory_readers/gps_trajectory_reader_base.hpp"
#include "./graph_readers/graph_reader_base.hpp"

// Different types of graph readers.
enum GraphReaderType {
  SEATTLE_SAMPLE_ROADNET,
  // [USER DEFINE]: Define your own graph reader types here.
};
static const char *GraphReaderTypeStrings[] = {
  "SEATTLE_SAMPLE_ROADNET",
  // [USER DEFINE]: Define your own graph reader types here.
};

// Get graph reader type from string name.
GraphReaderType getGraphReaderType(const std::string& graphReaderType);

// Different types of GPS trajectory readers.
enum GPSTrajectoryReaderType {
  SEATTLE_SAMPLE_GPS,
  // [USER DEFINE]: Define your own GPS trajectory reader types here.
};
static const char *GPSTrajectoryReaderTypeStrings[] = {
  "SEATTLE_SAMPLE_GPS",
  // [USER DEFINE]: Define your own GPS trajectory reader types here.
};

// Get GPS trajectory reader type from string name.
GPSTrajectoryReaderType getGPSTrajectoryReaderType(const std::string& gpsTrajectoryReaderType);

/**
 * A simple factory pattern interface for users to provide customized implementations of
 * 1. Graph Reader - Read graph from user defined file format;
 * 2. GPS Trajectory Reader - Read GPS trajectory from user defined file format;
 */
class Factory {
public:
  static std::shared_ptr<GraphReaderBase> getGraphReader(const GraphReaderType grType);
  static std::shared_ptr<GPSTrajectoryReaderBase> getGPSTrajectoryReader(
    const GPSTrajectoryReaderType grType
  );
};

#endif /* factory_hpp */
