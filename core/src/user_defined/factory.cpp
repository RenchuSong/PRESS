//
//  factory.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "factory.hpp"
#include <cassert>
#include <string>

#include "./gps_trajectory_readers/gps_trajectory_reader_seattle_sample.hpp"
// [USER DEFINE]: Include your own gps trajectory reader types here.

#include "./graph_readers/graph_reader_seattle_sample.hpp"
// [USER DEFINE]: Include your own graph reader types here.


std::shared_ptr<GraphReaderBase> Factory::getGraphReader(const GraphReaderType grType) {
  switch (grType) {
    case GraphReaderType::SEATTLE_SAMPLE_ROADNET: {
      return std::make_shared<GraphReaderSeattleSample>();
    }
    // [USER DEFINE]: Construct your own graph reader types here.
  }
  
  // Should not go to this point. Check above if you handled all graph reader types.
  assert(false);
}

std::shared_ptr<GPSTrajectoryReaderBase> Factory::getGPSTrajectoryReader(
  const GPSTrajectoryReaderType grType
) {
  switch (grType) {
    case GPSTrajectoryReaderType::SEATTLE_SAMPLE_GPS: {
      return std::make_shared<GPSTrajectoryReaderSeattleSample>();
    }
    // [USER DEFINE]: Construct your own gps trajectory reader types here.
  }
  
  // Should not go to this point. Check above if you handled all gps trajectory reader types.
  assert(false);
}
