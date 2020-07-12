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

#endif /* test_helper_hpp */
