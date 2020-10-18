//
//  test_helper.hpp
//  press-v3
//
//  Created by Renchu Song on 7/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef test_helper_hpp
#define test_helper_hpp

#include "gtest/gtest.h"
#include <vector>

#include "../helper.hpp"

TEST(HelperTest, Deg2RadTest) {
  EXPECT_DOUBLE_EQ(0, deg2rad(0));
  EXPECT_DOUBLE_EQ(M_PI / 4, deg2rad(45));
  EXPECT_DOUBLE_EQ(M_PI / 2, deg2rad(90));
  EXPECT_DOUBLE_EQ(M_PI, deg2rad(180));
  EXPECT_DOUBLE_EQ(M_PI * 1.25, deg2rad(225));
  EXPECT_DOUBLE_EQ(M_PI * 2, deg2rad(360));
  EXPECT_DOUBLE_EQ(-M_PI / 4, deg2rad(-45));
  EXPECT_DOUBLE_EQ(-M_PI / 2, deg2rad(-90));
  EXPECT_DOUBLE_EQ(-M_PI, deg2rad(-180));
  EXPECT_DOUBLE_EQ(-M_PI * 1.25, deg2rad(-225));
  EXPECT_DOUBLE_EQ(-M_PI * 2, deg2rad(-360));
}

TEST(HelperTest, DistPoint2IntervalTest) {
  Point2D p1(0, 1);
  Point2D p2(0.5, 1);
  Point2D p3(1, 1);
  Point2D p4(1.5, 1);
  Point2D p5(2, 1);
  Point2D p6(0.5, 0);
  Point2D p7(1.5, 0);
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p1, p2, p4));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p2, p2, p4));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p3, p2, p4));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p4, p2, p4));
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p5, p2, p4));
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p1, p4, p2));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p2, p4, p2));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p3, p4, p2));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p4, p4, p2));
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p5, p4, p2));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p1, p6, p7));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p2, p6, p7));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p3, p6, p7));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p4, p6, p7));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p5, p6, p7));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p1, p7, p6));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p2, p7, p6));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p3, p7, p6));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p4, p7, p6));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p5, p7, p6));
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p1, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(0.5), distPoint2Interval(p6, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(0.125), distPoint2Interval(p3, p2, p7));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p7, p2, p7));
  EXPECT_DOUBLE_EQ(0.5, distPoint2Interval(p1, p7, p2));
  EXPECT_DOUBLE_EQ(sqrt(0.5), distPoint2Interval(p6, p7, p2));
  EXPECT_DOUBLE_EQ(sqrt(0.125), distPoint2Interval(p3, p7, p2));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p7, p7, p2));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p1, p6, p6));
  EXPECT_DOUBLE_EQ(1, distPoint2Interval(p2, p6, p6));
  EXPECT_DOUBLE_EQ(sqrt(1.25), distPoint2Interval(p3, p6, p6));
  EXPECT_DOUBLE_EQ(sqrt(2), distPoint2Interval(p4, p6, p6));
  EXPECT_DOUBLE_EQ(sqrt(3.25), distPoint2Interval(p5, p6, p6));
  EXPECT_DOUBLE_EQ(0, distPoint2Interval(p6, p6, p6));
}

TEST(HelperTest, DistPoint2PolylineTest) {
  std::vector<Point2D> polyline {
    Point2D(0, 0),
    Point2D(1, 1),
    Point2D(3, 1),
    Point2D(4, 3),
    Point2D(3, 5)
  };
  EXPECT_DOUBLE_EQ(0, distPoint2Polyline(Point2D(0, 0), polyline));
  EXPECT_DOUBLE_EQ(1, distPoint2Polyline(Point2D(-1, 0), polyline));
  EXPECT_DOUBLE_EQ(0, distPoint2Polyline(Point2D(0.5, 0.5), polyline));
  EXPECT_DOUBLE_EQ(0.2, distPoint2Polyline(Point2D(2, 0.8), polyline));
  EXPECT_DOUBLE_EQ(1, distPoint2Polyline(Point2D(3, 0), polyline));
  EXPECT_NEAR(2/sqrt(5), distPoint2Polyline(Point2D(4, 1), polyline), 1e-8);
  EXPECT_NEAR(1/sqrt(5), distPoint2Polyline(Point2D(3, 4), polyline), 1e-8);
}

TEST(HelperTest, DistProjAlongGeoTest) {
  std::vector<Point2D> geo {
    Point2D(1, 1),
    Point2D(2, 1),
    Point2D(3, 1.5),
    Point2D(3, 2),
    Point2D(2, 3),
    Point2D(2, 4),
    Point2D(4, 5),
    Point2D(5, 5),
  };
  EXPECT_DOUBLE_EQ(0, distProjAlongGeo(Point2D(1, 1), geo));
  EXPECT_DOUBLE_EQ(0.5, distProjAlongGeo(Point2D(1.5, 1), geo));
  EXPECT_DOUBLE_EQ(1, distProjAlongGeo(Point2D(2, 1), geo));
  EXPECT_DOUBLE_EQ(1 + sqrt(5.0 / 16), distProjAlongGeo(Point2D(2.5, 1.25), geo));
  EXPECT_DOUBLE_EQ(1 + sqrt(1.25), distProjAlongGeo(Point2D(3, 1.5), geo));
  EXPECT_DOUBLE_EQ(1.5 + sqrt(1.25), distProjAlongGeo(Point2D(3, 2), geo));
  EXPECT_DOUBLE_EQ(1.5 + sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(2, 3), geo));
  EXPECT_DOUBLE_EQ(2 + sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(2, 3.5), geo));
  EXPECT_DOUBLE_EQ(2.5 + sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(2, 4), geo));
  EXPECT_DOUBLE_EQ(2.5 + 2 * sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(3, 4.5), geo));
  EXPECT_DOUBLE_EQ(2.5 + 3 * sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(4, 5), geo));
  EXPECT_DOUBLE_EQ(3.5 + 3 * sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(5, 5), geo));
  EXPECT_DOUBLE_EQ(0, distProjAlongGeo(Point2D(1, 1.5), geo));
  EXPECT_DOUBLE_EQ(0, distProjAlongGeo(Point2D(1, 2), geo));
  EXPECT_DOUBLE_EQ(0, distProjAlongGeo(Point2D(1, 0), geo));
  EXPECT_DOUBLE_EQ(1, distProjAlongGeo(Point2D(2, 0), geo));
  EXPECT_DOUBLE_EQ(1 + 1 / (4 * sqrt(1.25)), distProjAlongGeo(Point2D(2, 1.5), geo));
  EXPECT_DOUBLE_EQ(1 + sqrt(1.25), distProjAlongGeo(Point2D(4, 1.5), geo));
  EXPECT_DOUBLE_EQ(1.25 + sqrt(1.25), distProjAlongGeo(Point2D(4, 1.75), geo));
  EXPECT_DOUBLE_EQ(1.25 + sqrt(1.25), distProjAlongGeo(Point2D(2.9, 1.75), geo));
  EXPECT_DOUBLE_EQ(3 + 3 * sqrt(1.25) + sqrt(2), distProjAlongGeo(Point2D(4.5, 5), geo));
  EXPECT_DOUBLE_EQ(2.5 + 3 * sqrt(1.25) + sqrt(2) - 0.5 / sqrt(1.25), distProjAlongGeo(Point2D(4, 4), geo));
}

TEST(HelperTest, DistProjAlongIntervalTest) {
  Point2D p1(0, 1);
  Point2D p2(0.5, 1);
  Point2D p3(1, 1);
  Point2D p4(1.5, 1);
  Point2D p5(2, 1);
  Point2D p6(0.5, 0);
  Point2D p7(1.5, 0);
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p1, p2, p4));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p2, p2, p4));
  EXPECT_DOUBLE_EQ(0.5, distProjAlongInterval(p3, p2, p4));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p4, p2, p4));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p5, p2, p4));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p1, p4, p2));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p2, p4, p2));
  EXPECT_DOUBLE_EQ(0.5, distProjAlongInterval(p3, p4, p2));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p4, p4, p2));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p5, p4, p2));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p1, p6, p7));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p2, p6, p7));
  EXPECT_DOUBLE_EQ(0.5, distProjAlongInterval(p3, p6, p7));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p4, p6, p7));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p5, p6, p7));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p1, p7, p6));
  EXPECT_DOUBLE_EQ(1, distProjAlongInterval(p2, p7, p6));
  EXPECT_DOUBLE_EQ(0.5, distProjAlongInterval(p3, p7, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p4, p7, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p5, p7, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p1, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(0.5), distProjAlongInterval(p6, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(0.125), distProjAlongInterval(p3, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(2), distProjAlongInterval(p7, p2, p7));
  EXPECT_DOUBLE_EQ(sqrt(2), distProjAlongInterval(p1, p7, p2));
  EXPECT_DOUBLE_EQ(sqrt(0.5), distProjAlongInterval(p6, p7, p2));
  EXPECT_DOUBLE_EQ(sqrt(2) - sqrt(0.125), distProjAlongInterval(p3, p7, p2));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p7, p7, p2));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p1, p6, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p2, p6, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p3, p6, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p4, p6, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p5, p6, p6));
  EXPECT_DOUBLE_EQ(0, distProjAlongInterval(p6, p6, p6));
}

TEST(HelperTest, EuclideanDistanceTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 1);
  Point2D p3(3, 4);
  EXPECT_DOUBLE_EQ(0, euclideanDistance(p1, p1));
  EXPECT_DOUBLE_EQ(1, euclideanDistance(p1, p2));
  EXPECT_DOUBLE_EQ(5, euclideanDistance(p1, p3));
}

TEST(HelperTest, GPS2Point2DTest) {
  GPSPoint g1(0, 0, 0);
  auto p1 = gps2Point2D(g1);
  EXPECT_DOUBLE_EQ(0, p1.x);
  EXPECT_DOUBLE_EQ(0, p1.y);
  GPSPoint g2(0, 0, 10);
  auto p2 = gps2Point2D(g2);
  EXPECT_DOUBLE_EQ(10 * 111319.49, p2.x);
  EXPECT_DOUBLE_EQ(0, p2.y);
  GPSPoint g3(0, 0, 180);
  auto p3 = gps2Point2D(g3);
  EXPECT_DOUBLE_EQ(180 * 111319.49, p3.x);
  EXPECT_DOUBLE_EQ(0, p3.y);
  GPSPoint g4(0, 0, -10);
  auto p4 = gps2Point2D(g4);
  EXPECT_DOUBLE_EQ(-10 * 111319.49, p4.x);
  EXPECT_DOUBLE_EQ(0, p4.y);
  GPSPoint g5(0, 0, -180);
  auto p5 = gps2Point2D(g5);
  EXPECT_DOUBLE_EQ(-180 * 111319.49, p5.x);
  EXPECT_DOUBLE_EQ(0, p5.y);
  
  GPSPoint g6(0, 45, 0);
  auto p6 = gps2Point2D(g6);
  EXPECT_DOUBLE_EQ(0, p6.x);
  EXPECT_DOUBLE_EQ(111195.19 * 45, p6.y);
  GPSPoint g7(0, 45, 10);
  auto p7 = gps2Point2D(g7);
  EXPECT_DOUBLE_EQ(10 * 111319.49 / sqrt(2), p7.x);
  EXPECT_DOUBLE_EQ(111195.19 * 45, p7.y);
  GPSPoint g8(0, 45, 180);
  auto p8 = gps2Point2D(g8);
  EXPECT_DOUBLE_EQ(180 * 111319.49 / sqrt(2), p8.x);
  EXPECT_DOUBLE_EQ(111195.19 * 45, p8.y);
  GPSPoint g9(0, 45, -10);
  auto p9 = gps2Point2D(g9);
  EXPECT_DOUBLE_EQ(-10 * 111319.49 / sqrt(2), p9.x);
  EXPECT_DOUBLE_EQ(111195.19 * 45, p9.y);
  GPSPoint g10(0, 45, -180);
  auto p10 = gps2Point2D(g10);
  EXPECT_DOUBLE_EQ(-180 * 111319.49 / sqrt(2), p10.x);
  EXPECT_DOUBLE_EQ(111195.19 * 45, p10.y);
  
  GPSPoint g11(0, -45, 0);
  auto p11 = gps2Point2D(g11);
  EXPECT_DOUBLE_EQ(0, p11.x);
  EXPECT_DOUBLE_EQ(-111195.19 * 45, p11.y);
  GPSPoint g12(0, -45, 10);
  auto p12 = gps2Point2D(g12);
  EXPECT_DOUBLE_EQ(10 * 111319.49 / sqrt(2), p12.x);
  EXPECT_DOUBLE_EQ(-111195.19 * 45, p12.y);
  GPSPoint g13(0, -45, 180);
  auto p13 = gps2Point2D(g13);
  EXPECT_DOUBLE_EQ(180 * 111319.49 / sqrt(2), p13.x);
  EXPECT_DOUBLE_EQ(-111195.19 * 45, p13.y);
  GPSPoint g14(0, -45, -10);
  auto p14 = gps2Point2D(g14);
  EXPECT_DOUBLE_EQ(-10 * 111319.49 / sqrt(2), p14.x);
  EXPECT_DOUBLE_EQ(-111195.19 * 45, p14.y);
  GPSPoint g15(0, -45, -180);
  auto p15 = gps2Point2D(g15);
  EXPECT_DOUBLE_EQ(-180 * 111319.49 / sqrt(2), p15.x);
  EXPECT_DOUBLE_EQ(-111195.19 * 45, p15.y);
  
  GPSPoint g16(0, 90, 0);
  auto p16 = gps2Point2D(g16);
  EXPECT_DOUBLE_EQ(0, p16.x);
  EXPECT_DOUBLE_EQ(111195.19 * 90, p16.y);
  GPSPoint g17(0, 90, 10);
  auto p17 = gps2Point2D(g17);
  EXPECT_NEAR(0, p17.x, 1e-8);
  EXPECT_DOUBLE_EQ(111195.19 * 90, p17.y);

  GPSPoint g18(0, -90, 0);
  auto p18 = gps2Point2D(g18);
  EXPECT_DOUBLE_EQ(0, p18.x);
  EXPECT_DOUBLE_EQ(111195.19 * -90, p18.y);
  GPSPoint g19(0, -90, -180);
  auto p19 = gps2Point2D(g19);
  EXPECT_NEAR(0, p19.x, 1e-8);
  EXPECT_DOUBLE_EQ(-111195.19 * 90, p19.y);
}

TEST(HelperTest, IntervalIntersectTest) {
  Point2D p1(1, 1);
  Point2D p2(2, 1);
  Point2D p3(3, 1);
  Point2D p4(1, 2);
  Point2D p5(2, 2);
  Point2D p6(3, 2);
  Point2D p7(4, 2);
  Point2D p8(1, 3);
  Point2D p9(2, 3);
  Point2D p10(3, 3);
  Point2D p11(4, 4);

  EXPECT_TRUE(intervalIntersect(p4, p6, p2, p9));
  EXPECT_TRUE(intervalIntersect(p8, p3, p1, p10));
  EXPECT_TRUE(intervalIntersect(p8, p6, p2, p11));
  EXPECT_TRUE(intervalIntersect(p1, p2, p3, p2));
  EXPECT_TRUE(intervalIntersect(p4, p7, p5, p6));
  EXPECT_TRUE(intervalIntersect(p8, p5, p5, p10));
  EXPECT_TRUE(intervalIntersect(p1, p10, p5, p11));

  EXPECT_FALSE(intervalIntersect(p4, p5, p6, p7));
  EXPECT_FALSE(intervalIntersect(p1, p3, p5, p11));
}

TEST(HelperTest, IntervalThroughMBRTest) {
  Point2D p1(1, 1);
  Point2D p2(2, 1);
  Point2D p3(3, 1);
  Point2D p4(1, 2);
  Point2D p5(2, 2);
  Point2D p6(3, 2);
  Point2D p7(4, 2);
  Point2D p8(1, 3);
  Point2D p9(2, 3);
  Point2D p10(3, 3);
  Point2D p11(4, 4);

  EXPECT_TRUE(intervalThroughMBR(p5, p7, p1, p10));
  EXPECT_TRUE(intervalThroughMBR(p1, p7, p2, p10));
  EXPECT_TRUE(intervalThroughMBR(p1, p11, p5, p10));
  EXPECT_TRUE(intervalThroughMBR(p8, p3, p2, p6));
  EXPECT_TRUE(intervalThroughMBR(p8, p11, p1, p10));
  EXPECT_TRUE(intervalThroughMBR(p6, p7, p2, p10));
  EXPECT_TRUE(intervalThroughMBR(p6, p7, p5, p10));
  EXPECT_TRUE(intervalThroughMBR(p4, p7, p5, p10));
  EXPECT_TRUE(intervalThroughMBR(p1, p11, p4, p9));
  EXPECT_TRUE(intervalThroughMBR(p8, p3, p1, p5));

  EXPECT_FALSE(intervalThroughMBR(p8, p11, p2, p10));
  EXPECT_FALSE(intervalThroughMBR(p8, p9, p2, p6));
}

TEST(HelperTest, LinearInterpolateTest) {
  EXPECT_DOUBLE_EQ(2, linearInterpolate(1, 2, 2, 4, 1));
  EXPECT_DOUBLE_EQ(2.2, linearInterpolate(1, 2, 2, 4, 1.1));
  EXPECT_DOUBLE_EQ(3.8, linearInterpolate(1, 2, 2, 4, 1.9));
  EXPECT_DOUBLE_EQ(4, linearInterpolate(1, 2, 2, 4, 2));
  EXPECT_DOUBLE_EQ(2, linearInterpolate(1, 2, 1, 4, 1));
}

TEST(HelperTest, PointInMBRTest) {
  Point2D p1(-1, -2);
  Point2D p2(3, 4);
  Point2D p3(-1, 4);
  Point2D p4(3, -2);
  Point2D p5(-1, 0);
  Point2D p6(3, 0);
  Point2D p7(0, -2);
  Point2D p8(0, 4);
  Point2D p9(0, 0);
  Point2D p10(-2, 0);
  Point2D p11(5, 0);
  Point2D p12(0, -4);
  Point2D p13(0, 8);

  EXPECT_TRUE(pointInMBR(p1, p1, p2));
  EXPECT_TRUE(pointInMBR(p2, p1, p2));
  EXPECT_TRUE(pointInMBR(p3, p1, p2));
  EXPECT_TRUE(pointInMBR(p4, p1, p2));
  EXPECT_TRUE(pointInMBR(p5, p1, p2));
  EXPECT_TRUE(pointInMBR(p6, p1, p2));
  EXPECT_TRUE(pointInMBR(p7, p1, p2));
  EXPECT_TRUE(pointInMBR(p8, p1, p2));
  EXPECT_TRUE(pointInMBR(p9, p1, p2));
  
  EXPECT_FALSE(pointInMBR(p10, p1, p2));
  EXPECT_FALSE(pointInMBR(p11, p1, p2));
  EXPECT_FALSE(pointInMBR(p12, p1, p2));
  EXPECT_FALSE(pointInMBR(p13, p1, p2));
}

// Check if a point is on an interval.
TEST(HelperTest, PointOnIntervalTest) {
  Point2D p1(1, 1);
  Point2D p2(2, 1);
  Point2D p3(3, 1);
  Point2D p4(1, 2);
  Point2D p5(2, 2);
  Point2D p6(3, 2);
  Point2D p7(4, 2);
  Point2D p8(1, 3);
  Point2D p9(2, 3);
  Point2D p10(3, 3);
  Point2D p11(4, 4);
  
  EXPECT_TRUE(pointOnInterval(p4, p4, p7));
  EXPECT_TRUE(pointOnInterval(p5, p4, p7));
  EXPECT_TRUE(pointOnInterval(p6, p4, p7));
  EXPECT_TRUE(pointOnInterval(p7, p4, p7));
  EXPECT_TRUE(pointOnInterval(p1, p1, p11));
  EXPECT_TRUE(pointOnInterval(p5, p1, p11));
  EXPECT_TRUE(pointOnInterval(p10, p1, p11));
  EXPECT_TRUE(pointOnInterval(p11, p1, p11));

  EXPECT_FALSE(pointOnInterval(p1, p5, p6));
  EXPECT_FALSE(pointOnInterval(p2, p5, p6));
  EXPECT_FALSE(pointOnInterval(p3, p5, p6));
  EXPECT_FALSE(pointOnInterval(p4, p5, p6));
  EXPECT_FALSE(pointOnInterval(p7, p5, p6));
  EXPECT_FALSE(pointOnInterval(p8, p3, p11));
  EXPECT_FALSE(pointOnInterval(p9, p8, p7));
}

TEST(HelperTest, Point2D2GPSTest) {
  std::vector<GPSPoint> gpsPoints {
    GPSPoint(10, 44.1, -122.5),
    GPSPoint(10, -44.1, 122.5),
    GPSPoint(10, 0, 0),
    GPSPoint(10, 89.999, 180),
    GPSPoint(10, -89.999, -180)
  };
  for (auto& gps: gpsPoints) {
    auto point = gps2Point2D(gps);
    auto convertedBackGPS = point2D2GPS(point, 0);
    EXPECT_DOUBLE_EQ(gps.latitude, convertedBackGPS.latitude);
    EXPECT_DOUBLE_EQ(gps.longitude, convertedBackGPS.longitude);
  }
}

TEST(HelperTest, PositionAlongPolylineTest) {
  std::vector<Point2D> shape {
    Point2D(1, 1),
    Point2D(3, 1),
    Point2D(4, 3),
    Point2D(5, 2)
  };
  Point2D result;
  positionAlongPolyline(shape, 0, result);
  EXPECT_DOUBLE_EQ(1, result.x);
  EXPECT_DOUBLE_EQ(1, result.y);
  positionAlongPolyline(shape, 1.5, result);
  EXPECT_DOUBLE_EQ(2.5, result.x);
  EXPECT_DOUBLE_EQ(1, result.y);
  positionAlongPolyline(shape, 2, result);
  EXPECT_DOUBLE_EQ(3, result.x);
  EXPECT_DOUBLE_EQ(1, result.y);
  positionAlongPolyline(shape, 2 + sqrt(5) / 2, result);
  EXPECT_DOUBLE_EQ(3.5, result.x);
  EXPECT_DOUBLE_EQ(2, result.y);
  positionAlongPolyline(shape, 2 + sqrt(5), result);
  EXPECT_DOUBLE_EQ(4, result.x);
  EXPECT_DOUBLE_EQ(3, result.y);
  positionAlongPolyline(shape, 2 + sqrt(5) + sqrt(2), result);
  EXPECT_DOUBLE_EQ(5, result.x);
  EXPECT_DOUBLE_EQ(2, result.y);
}

TEST(HelperTest, ScalarProductTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 2);
  Point2D p3(2, 0);
  Point2D p4(2, 2);
  EXPECT_DOUBLE_EQ(4, scalarProduct(p1, p3, p2, p4));
  EXPECT_DOUBLE_EQ(-4, scalarProduct(p1, p3, p4, p2));
  EXPECT_DOUBLE_EQ(0, scalarProduct(p1, p3, p1, p2));
  EXPECT_DOUBLE_EQ(0, scalarProduct(p1, p2, p1, p3));
  EXPECT_DOUBLE_EQ(4, scalarProduct(p1, p3, p1, p4));
  EXPECT_DOUBLE_EQ(4, scalarProduct(p1, p4, p1, p3));
  EXPECT_DOUBLE_EQ(0, scalarProduct(p2, p3, p1, p4));
  EXPECT_DOUBLE_EQ(0, scalarProduct(p1, p4, p2, p3));
  EXPECT_DOUBLE_EQ(-4, scalarProduct(p1, p3, p3, p2));
  EXPECT_DOUBLE_EQ(-4, scalarProduct(p3, p2, p1, p3));
}

TEST(HelperTest, SlopeTest) {
  EXPECT_DOUBLE_EQ(0, slope(0, 0, 1, 0));
  EXPECT_DOUBLE_EQ(1, slope(0, 0, 1, 1));
  EXPECT_DOUBLE_EQ(-1, slope(0, 0, 1, -1));
  EXPECT_DOUBLE_EQ(1e100, slope(1, 0, -1, 1));
  EXPECT_DOUBLE_EQ(1e100, slope(1, 0, 1, 1));
  EXPECT_DOUBLE_EQ(-1e100, slope(1, 0, 1, -1));
  EXPECT_DOUBLE_EQ(1e100, slope(1, 1, -1, 1));
  EXPECT_DOUBLE_EQ(1e100, slope(1, 1, 1, 1));
  EXPECT_DOUBLE_EQ(-1e100, slope(1, 1, 1, -1));
}

TEST(HelperTest, VectorProductTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 1);
  Point2D p3(1, 0);
  Point2D p4(1, 1);
  EXPECT_DOUBLE_EQ(0, vectorProduct(p1, p3, p2, p4));
  EXPECT_DOUBLE_EQ(0, vectorProduct(p1, p3, p4, p2));
  EXPECT_DOUBLE_EQ(1, vectorProduct(p1, p3, p1, p2));
  EXPECT_DOUBLE_EQ(-1, vectorProduct(p1, p2, p1, p3));
  EXPECT_DOUBLE_EQ(1, vectorProduct(p1, p3, p1, p4));
  EXPECT_DOUBLE_EQ(-1, vectorProduct(p1, p4, p1, p3));
  EXPECT_DOUBLE_EQ(2, vectorProduct(p2, p3, p1, p4));
  EXPECT_DOUBLE_EQ(-2, vectorProduct(p1, p4, p2, p3));
  EXPECT_DOUBLE_EQ(1, vectorProduct(p1, p3, p3, p2));
  EXPECT_DOUBLE_EQ(-1, vectorProduct(p3, p2, p1, p3));
}

#endif /* test_helper_hpp */
