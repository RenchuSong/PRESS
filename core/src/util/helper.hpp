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
float euclideanDistance(const Point2D& p1, const Point2D& p2);

// Calculate the slope between two points.
double slope(double x1, double y1, double x2, double y2);

#endif /* helper_hpp */
