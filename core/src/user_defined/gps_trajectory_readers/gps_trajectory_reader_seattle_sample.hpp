//
//  gps_trajectory_reader_seattle_sample.hpp
//  press-v3
//
//  Created by Renchu Song on 9/7/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef gps_trajectory_reader_seattle_sample_hpp
#define gps_trajectory_reader_seattle_sample_hpp

#include "./gps_trajectory_reader_base.hpp"

class GPSTrajectoryReaderSeattleSample: public GPSTrajectoryReaderBase {
public:
  void readGPSTrajectoryImpl(const std::string& fileName, GPSTrajectory& gpsTrajectory);
};

#endif /* gps_trajectory_reader_seattle_sample_hpp */
