//
//  helper.cpp
//  press-v3
//
//  Created by Renchu Song on 6/21/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "helper.hpp"

// Euclidean distance between 2 points.
float euclideanDistance(const Point2D& p1, const Point2D& p2) {
  return sqrtf((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Calculate the slope between two points.
double slope(double x1, double y1, double x2, double y2) {
  if (x1 >= x2) {
    if (y1 <= y2) {
      return 1e100;
    }
    return -1e100;
  }
  return (y2 - y1) / (x2 - x1);
}
