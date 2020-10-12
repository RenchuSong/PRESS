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
#include "../../topology/geospatial_structures.hpp"
#include "../../topology/trajectory.hpp"
#include "../../util/helper.hpp"
#include "../trajectory_reformattor.hpp"

TEST(TrajectoryReformattorTest, GPSTrajPRESSTrajLenMismatchTest) {
  MockTestObjects mockObj;
  TrajectoryReformatter trajFormatter;
  PRESSTrajectory pressTrajectory;
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
      mmTrajectory,
      pressTrajectory
    );
  }, "Assertion failed.*");
}

TEST(TrajectoryReformattorTest, EmptyTrajectoryTest) {
  MockTestObjects mockObj;
  TrajectoryReformatter trajFormatter;
  std::vector<GPSPoint> gpsSequence;
  GPSTrajectory gpsTrajectory(gpsSequence);
  std::vector<int> spSequence;
  MapMatchedTrajectory mmTrajectory(spSequence);
  PRESSTrajectory trajectory;
  trajFormatter.generateTrajectory(
    *(mockObj.spTable),
    *(mockObj.graph),
    gpsTrajectory,
    mmTrajectory,
    trajectory
  );
  EXPECT_EQ(0, trajectory.getSpatialLength());
  EXPECT_EQ(0, trajectory.getTemporalLength());
  EXPECT_EQ(0, trajectory.getSpatialComponent().size());
  EXPECT_EQ(0, trajectory.getTemporalComponent().size());
}

TEST(TrajectoryReformattorTest, TrajectoryWithOnePointTest) {
  MockTestObjects mockObj;
  TrajectoryReformatter trajFormatter;
  std::vector<GPSPoint> gpsSequence { GPSPoint(10, 37.2, 122.05) };
  GPSTrajectory gpsTrajectory(gpsSequence);
  std::vector<int> spSequence { 2 };
  MapMatchedTrajectory mmTrajectory(spSequence);
  PRESSTrajectory trajectory;
  trajFormatter.generateTrajectory(
    *(mockObj.spTable),
    *(mockObj.graph),
    gpsTrajectory,
    mmTrajectory,
    trajectory
  );
  EXPECT_EQ(1, trajectory.getSpatialLength());
  EXPECT_EQ(1, trajectory.getTemporalLength());
  EXPECT_EQ(2, trajectory.getSpatialComponent().at(0));
  GPSPoint gps1(0, 37.2, 122.05);
  GPSPoint gps2(0, 37.2, 122.1);
  EXPECT_DOUBLE_EQ(
    euclideanDistance(
      gps2Point2D(gps1),
      gps2Point2D(gps2)
    ),
    trajectory.getTemporalComponent().at(0).dist
  );
  EXPECT_DOUBLE_EQ(10, trajectory.getTemporalComponent().at(0).t);
}

TEST(TrajectoryReformattorTest, TrajectoryTest) {
  MockTestObjects mockObj;
  TrajectoryReformatter trajFormatter;
  std::vector<GPSPoint> gpsSequence {
    GPSPoint(1, 37.0, 122.0),
    GPSPoint(2, 37.05, 122.1),
    GPSPoint(3, 37.075, 122.15),
    GPSPoint(4, 37.15, 122.15),
    GPSPoint(5, 37.15, 122.15),
    GPSPoint(7, 37.125, 122.175), // backward
    GPSPoint(10, 37.35, 122.25),  // skip some edge
    GPSPoint(11, 37.4, 122.2),
  };
  GPSTrajectory gpsTrajectory(gpsSequence);
  std::vector<int> spSequence { 4, 4, 4, 6, 6, 6, 14, 14 };
  MapMatchedTrajectory mmTrajectory(spSequence);
  PRESSTrajectory trajectory;
  trajFormatter.generateTrajectory(
    *(mockObj.spTable),
    *(mockObj.graph),
    gpsTrajectory,
    mmTrajectory,
    trajectory
  );
  std::vector<int> expectedSpComp { 4, 6, 12, 14 };
  EXPECT_EQ(expectedSpComp.size(), trajectory.getSpatialLength());
  for (auto i = 0; i < expectedSpComp.size(); i++) {
    EXPECT_EQ(expectedSpComp.at(i), trajectory.getSpatialComponent().at(i));
  }
  std::vector<TemporalPair> expectedTprComp;
  double dist = 0;
  expectedTprComp.emplace_back(TemporalPair(1, dist));
  dist += euclideanDistance(gps2Point2D(gpsSequence.at(0)), gps2Point2D(gpsSequence.at(1)));
  expectedTprComp.emplace_back(TemporalPair(2, dist));
  dist += euclideanDistance(gps2Point2D(gpsSequence.at(1)), gps2Point2D(gpsSequence.at(2)));
  expectedTprComp.emplace_back(TemporalPair(3, dist));
  GPSPoint gps1(0, 37.1, 122.2);
  dist = euclideanDistance(gps2Point2D(gpsSequence.at(0)), gps2Point2D(gps1)) +
    euclideanDistance(gps2Point2D(gpsSequence.at(3)), gps2Point2D(gps1));
  expectedTprComp.emplace_back(TemporalPair(4, dist));
  expectedTprComp.emplace_back(TemporalPair(5, dist));
  expectedTprComp.emplace_back(TemporalPair(7, dist));
  GPSPoint gps2(0, 37.3, 122.3);
  dist = mockObj.graph->getEdge(4).getDistance() +
    mockObj.graph->getEdge(6).getDistance() +
    mockObj.graph->getEdge(12).getDistance();
  expectedTprComp.emplace_back(
    TemporalPair(
      10,
      dist + euclideanDistance(gps2Point2D(gps2), gps2Point2D(gpsSequence.at(6)))
    )
  );
  expectedTprComp.emplace_back(
    TemporalPair(
      11,
      dist + euclideanDistance(gps2Point2D(gps2), gps2Point2D(gpsSequence.at(7)))
    )
  );
  EXPECT_EQ(expectedTprComp.size(), trajectory.getTemporalLength());
  for (auto i = 0; i < trajectory.getTemporalLength(); i++) {
    EXPECT_DOUBLE_EQ(expectedTprComp.at(i).t, trajectory.getTemporalComponent().at(i).t);
    // Here we cannot use exact equal because there is small coordinate bias for different latitudes.
    EXPECT_NEAR(expectedTprComp.at(i).dist, trajectory.getTemporalComponent().at(i).dist, 0.01);
  }
}

#endif /* test_trajectory_reformattor_hpp */
