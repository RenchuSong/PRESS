//
//  auxiliaries.hpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef auxiliaries_hpp
#define auxiliaries_hpp

#include <iostream>

// A 2D point.
struct Point2D {
  double x;
  double y;
  
  Point2D(double x, double y): x(x), y(y) { }
  void print() {
    std::cout << "(" << x << ", " << y << ")";
  }
};

// A GPS point.
struct GPSPoint {
  double t;
  double latitude, longitude;
  
  GPSPoint(double t, double lat, double lng): t(t), latitude(lat), longitude(lng) { }
  void print() {
    std::cout << "(" << t << ": " << latitude << ", " << longitude << ")";
  }
};

// A temporal pair (t, d).
struct TemporalPair {
  double t;
  double dist;
  
  TemporalPair(double t, double dist): t(t), dist(dist) { }
  void print() {
    std::cout << "(" << t << ": " << dist << ")";
  }
};

#endif /* auxiliaries_hpp */
