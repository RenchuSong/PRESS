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

#include "../helper.hpp"

TEST(HelperTest, EuclideanDistanceTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 1);
  Point2D p3(3, 4);
  EXPECT_EQ(0, euclideanDistance(p1, p1));
  EXPECT_EQ(1, euclideanDistance(p1, p2));
  EXPECT_EQ(5, euclideanDistance(p1, p3));
}

TEST(HelperTest, SlopeTest) {
  EXPECT_EQ(0, slope(0, 0, 1, 0));
  EXPECT_EQ(1, slope(0, 0, 1, 1));
  EXPECT_EQ(-1, slope(0, 0, 1, -1));
}

TEST(HelperTest, VectorProductTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 1);
  Point2D p3(1, 0);
  Point2D p4(1, 1);
  EXPECT_EQ(0, vectorProduct(p1, p3, p2, p4));
  EXPECT_EQ(0, vectorProduct(p1, p3, p4, p2));
  EXPECT_EQ(1, vectorProduct(p1, p3, p1, p2));
  EXPECT_EQ(-1, vectorProduct(p1, p2, p1, p3));
  EXPECT_EQ(1, vectorProduct(p1, p3, p1, p4));
  EXPECT_EQ(-1, vectorProduct(p1, p4, p1, p3));
  EXPECT_EQ(2, vectorProduct(p2, p3, p1, p4));
  EXPECT_EQ(-2, vectorProduct(p1, p4, p2, p3));
  EXPECT_EQ(1, vectorProduct(p1, p3, p3, p2));
  EXPECT_EQ(-1, vectorProduct(p3, p2, p1, p3));
}

TEST(HelperTest, ScalarProductTest) {
  Point2D p1(0, 0);
  Point2D p2(0, 2);
  Point2D p3(2, 0);
  Point2D p4(2, 2);
  EXPECT_EQ(4, scalarProduct(p1, p3, p2, p4));
  EXPECT_EQ(-4, scalarProduct(p1, p3, p4, p2));
  EXPECT_EQ(0, scalarProduct(p1, p3, p1, p2));
  EXPECT_EQ(0, scalarProduct(p1, p2, p1, p3));
  EXPECT_EQ(4, scalarProduct(p1, p3, p1, p4));
  EXPECT_EQ(4, scalarProduct(p1, p4, p1, p3));
  EXPECT_EQ(0, scalarProduct(p2, p3, p1, p4));
  EXPECT_EQ(0, scalarProduct(p1, p4, p2, p3));
  EXPECT_EQ(-4, scalarProduct(p1, p3, p3, p2));
  EXPECT_EQ(-4, scalarProduct(p3, p2, p1, p3));
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

#endif /* test_helper_hpp */
