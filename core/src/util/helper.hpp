//
//  helper.hpp
//  press-v3
//
//  Created by Renchu Song on 10/31/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef helper_hpp
#define helper_hpp

#include <cmath>

#include "../topology/auxiliaries.hpp"

// Euclidean distance between 2 points.
double euclideanDistance(const Point2D& p1, const Point2D& p2) {
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

#endif /* helper_hpp */
