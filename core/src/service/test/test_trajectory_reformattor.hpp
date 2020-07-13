//
//  test_trajectory_reformattor.hpp
//  press-v3
//
//  Created by Renchu Song on 7/12/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef test_trajectory_reformattor_hpp
#define test_trajectory_reformattor_hpp

#include "gtest/gtest.h"

#include "../../test/mock_test_obj.hpp"
#include "../../topology/auxiliaries.hpp"
#include "../../topology/trajectory.hpp"
#include "../trajectory_reformattor.hpp"

TEST(TrajectoryReformattorTest, GPSTrajPRESSTrajLenMismatchTest) {
  MockTestObjects mockObj;
  TrajectoryReformatter trajFormatter;
  std::vector<GPSPoint> gpsSequence {
    GPSPoint(0, 0, 0)
  };
  GPSTrajectory gpsTrajectory(gpsSequence);
  std::vector<int> spSequence { 1, 2, 3 };
  MapMatchedTrajectory mmTrajectory(spSequence);
  EXPECT_DEATH({
    trajFormatter.generateTrajectory(
      *(mockObj.spTable),
      *(mockObj.graph),
      gpsTrajectory,
      mmTrajectory
    );
  }, "Assertion failed.*");
}

#endif /* test_trajectory_reformattor_hpp */
