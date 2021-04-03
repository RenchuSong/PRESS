//
//  geospatial_structures.hpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef geospatial_structures_hpp
#define geospatial_structures_hpp

#include <iostream>

// A 2D point.
struct Point2D {
  double x;
  double y;

  Point2D() { };
  Point2D(double x, double y) {
    setPosition(x, y);
  }
  Point2D(const Point2D& other) {
    setPosition(other.x, other.y);
  }
  void setPosition(double x, double y) {
    this->x = x;
    this->y = y;
  }
  void print() const {
    std::cout << "(" << x << ", " << y << ")";
  }
  std::string toJSONString() const {
    return std::string("{\"X\":") + std::to_string(x) + ",\"Y\":" + std::to_string(y) + "}";
  }
};

// A GPS point.
struct GPSPoint {
  double t;
  double latitude, longitude;

  GPSPoint(double lat, double lng): GPSPoint(0, lat, lng) { }
  GPSPoint(double t, double lat, double lng): t(t), latitude(lat), longitude(lng) { }
  void print() const {
    std::cout << "(" << t << ": " << latitude << ", " << longitude << ")";
  }
  std::string toJSONString() const {
    return std::string("{\"LAT\":") + std::to_string(latitude) + ",\"LONG\":" + std::to_string(longitude) + ",\"T\":" + std::to_string(t) + "}";
  }
};

// A temporal pair (t, d).
struct TemporalPair {
  double t;
  double dist;

  TemporalPair(double t, double dist): t(t), dist(dist) { }
  void print() const {
    std::cout << "(" << t << ": " << dist << ")";
  }
  std::string toJSONString() const {
    return std::string("{\"D\":") + std::to_string(dist) + ",\"T\":" + std::to_string(t) + "}";
  }
};

#endif /* geospatial_structures_hpp */
