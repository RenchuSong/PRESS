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
#include <vector>

#include "../topology/auxiliaries.hpp"

// Meters per latitude.
const double M_PER_LAT = 111195.19;

// Meters per longitude on equator.
const double M_PER_LONG = 111319.49;

// Convert degree to radian.
double deg2rad(double deg);

// Distance of the a 2D point to a 2D interval.
double distPoint2Interval(const Point2D& point, const Point2D& end1, const Point2D& end2);

// Distance of the projection of the point to the geometry along the geometry.
double distProjAlongGeo(const Point2D& point, const std::vector<Point2D>& geometry);

// Distance of the projection of the point to the interval along the interval.
double distProjAlongInterval(const Point2D& point, const Point2D& end1, const Point2D& end2);

// Euclidean distance between 2 points.
double euclideanDistance(const Point2D& p1, const Point2D& p2);

// Convert GPS point to 2D point.
Point2D gps2Point2D(const GPSPoint& gpsPoint);

// Check if 2 intervals intersect with each other in strict case/
// Strict means touch doesn't count as intersect.
bool intervalIntersect(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& p3,
  const Point2D& p4
);

// Check if interval [p1, p2] goes through MBR [[minBound], [maxBound]] in strict case.
// Strict means touch doesn't count as goes through.
bool intervalThroughMBR(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& minBound,
  const Point2D& maxBound
);

// Check if a point is within MBR x bound [minBound.x, maxBound.x], y bound [minBound.y, maxBound.y]
bool pointInMBR(const Point2D& point, const Point2D& minBound, const Point2D& maxBound);

// Check if a point is on an interval.
bool pointOnInterval(const Point2D& point, const Point2D& p1, const Point2D& p2);

// scalar product of two vectors
double scalarProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2);

// Calculate the slope between two points.
double slope(double x1, double y1, double x2, double y2);

// vector product of two vectors
double vectorProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2);

#endif /* helper_hpp */
