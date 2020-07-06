//
//  trajectory_reformattor.cpp
//  press-v3
//
//  Created by Renchu Song on 7/5/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>
#include <vector>

#include "trajectory_reformattor.hpp"
#include "../util/helper.hpp"

PRESSTrajectory TrajectoryReformatter::generateTrajectory(
  SPTable& spTable,
  Graph& graph,
  GPSTrajectory& gpsTrajectory,
  MapMatchedTrajectory &mmTrajectory
) {
  // GPS trajectory and map matched trajectory should have same length.
  assert (gpsTrajectory.getLength() == mmTrajectory.getLength());

  std::vector<int> spatialComponent;
  std::vector<TemporalPair> temporalComponent;

  // Complement the edge sequence to a complete path via shortest path table.
  // Assumption: vehicle doesn't drive backward.
  auto length = mmTrajectory.getLength();
  auto& mmSequence = mmTrajectory.getEdgeList();
  if (length > 0) {
    spatialComponent.emplace_back(mmSequence.at(0));
    for (auto i = 1; i < length; i++) {
      if (mmSequence.at(i) != mmSequence.at(i - 1)) {
        spTable.complement(graph, mmSequence.at(i - 1), mmSequence.at(i), spatialComponent);
      }
    }
  }

  // Get the temporal sequence.
  float dist = 0;
  int pt = 0;
  for (auto i = 0; i < length; i++) {
    // Accumulate the edges before current matched one.
    while (mmSequence.at(i) != spatialComponent.at(pt)) {
      dist += graph.getEdge(spatialComponent.at(pt)).getDistance();
      pt++;
    }
    // Project the GPS point to the edge with closest distance, and add the distance.
    auto& gpsPoint = gpsTrajectory.getGPSPoint(i);
    temporalComponent.push_back(TemporalPair(
      gpsPoint.t,
      dist + distProjAlongGeo(
        gps2Point2D(gpsPoint),
        graph.getEdge(spatialComponent.at(pt)).getShape()
      )
    ));
  }
  // Make the distance monotonic.
  for (auto i = 1; i < length; i++) {
    temporalComponent.at(i).dist = std::max(
      temporalComponent.at(i - 1).dist,
      temporalComponent.at(i).dist
    );
  }

  return PRESSTrajectory(spatialComponent, temporalComponent);
}

TrajectoryReformatter::~TrajectoryReformatter() { }
