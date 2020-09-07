//
//  gps_trajectory_reader_base.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef gps_trajectory_reader_base_hpp
#define gps_trajectory_reader_base_hpp

#include <vector>

#include "../../topology/trajectory.hpp"

class GPSTrajectoryReaderBase {
public:
  // The method for outside world to call: read GPS trajectory from given filename.
  void readGPSTrajectory(const std::string& fileName, GPSTrajectory& gpsTrajectory);

  // Each subclass needs to implement this method and read the GPS points from the filename.
  virtual void readGPSTrajectoryImpl(const std::string& fileName, GPSTrajectory& gpsTrajectory) = 0;
  
private:
  // Do sanity check of the GPS trajectory.
  void sanityCheck(GPSTrajectory& gpsTrajectory) const;
};

#endif /* gps_trajectory_reader_base_hpp */
