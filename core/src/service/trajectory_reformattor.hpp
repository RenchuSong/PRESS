//
//  trajectory_reformattor.hpp
//  press-v3
//
//  Created by Renchu Song on 7/5/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef trajectory_reformattor_hpp
#define trajectory_reformattor_hpp

#include "../topology/trajectory.hpp"
#include "../topology/sp_table.hpp"

class TrajectoryReformatter {
public:
  // Generate a PRESS trajectory from a map matched trajectory.
  PRESSTrajectory generateTrajectory(
    SPTable& spTable,
    Graph& graph,
    GPSTrajectory& gpsTrajectory,
    MapMatchedTrajectory& mmTrajectory
  );

  ~TrajectoryReformatter();
};

#endif /* trajectory_reformattor_hpp */
