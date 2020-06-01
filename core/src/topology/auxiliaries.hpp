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
  float x;
  float y;
  
  Point2D(float x, float y): x(x), y(y) { }
  void print() {
    std::cout << "(" << x << ", " << y << ")";
  }
};

// A GPS point.
struct GPSPoint {
  int t;
  float latitude, longitude;
  
  GPSPoint(int t, float lat, float lng): t(t), latitude(lat), longitude(lng) { }
  void print() {
    std::cout << "(" << t << ": " << latitude << ", " << longitude << ")";
  }
};

// A temporal pair (t, d).
struct TemporalPair {
  float t;
  float dist;
  
  TemporalPair(float t, float dist): t(t), dist(dist) { }
  void print() {
    std::cout << "(" << t << ": " << dist << ")";
  }
};

#endif /* auxiliaries_hpp */
