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

#include "../topology/geospatial_structures.hpp"

// Meters per latitude.
const double M_PER_LAT = 111195.19;

// Meters per longitude on equator.
const double M_PER_LONG = 111319.49;

// Convert degree to radian.
double deg2rad(double deg);

// Distance of the a 2D point to a 2D interval.
double distPoint2Interval(const Point2D& point, const Point2D& end1, const Point2D& end2);

// Distance from a 2D point to a polyline (edge shape).
double distPoint2Polyline(const Point2D& point, const std::vector<Point2D>& polyline);

// Distance of the projection of the point to the geometry along the geometry.
double distProjAlongGeo(const Point2D& point, const std::vector<Point2D>& geometry);

// Distance of the projection of the point to the interval along the interval.
double distProjAlongInterval(const Point2D& point, const Point2D& end1, const Point2D& end2);

// Euclidean distance between 2 points.
double euclideanDistance(const Point2D& p1, const Point2D& p2);

// Extend MBR.
void extendMBR(std::pair<Point2D, Point2D>& mbr, const Point2D& other);

// Convert GPS point to 2D point.
Point2D gps2Point2D(const GPSPoint& gpsPoint);

// Check if 2 intervals intersect with each other in loose case.
// Loose means touch counts as going through.
bool intervalIntersect(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& p3,
  const Point2D& p4
);

// Check if interval [p1, p2] goes through MBR [[minBound], [maxBound]] in loose case.
// Loose means touch counts as going through.
bool intervalThroughMBR(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& minBound,
  const Point2D& maxBound
);

// Linear interpolate: two points (x1, y1) and (x2, y2), get linear interpolated result at x.
double linearInterpolate(double x1, double y1, double x2, double y2, double x);

// Check if two MBRs intersect.
bool mbrIntersectWithMBR(
  const Point2D& low1,
  const Point2D& high1,
  const Point2D& low2,
  const Point2D& high2
);

// Check if a point is within MBR x bound [minBound.x, maxBound.x], y bound [minBound.y, maxBound.y]
bool pointInMBR(const Point2D& point, const Point2D& minBound, const Point2D& maxBound);

// Check if a point is on an interval.
bool pointOnInterval(const Point2D& point, const Point2D& p1, const Point2D& p2);

// Convert 2D point to GPS point.
GPSPoint point2D2GPS(const Point2D& point, double t);

// Get the 2D position of dist along with the polyline in definition direction.
void positionAlongPolyline(const std::vector<Point2D>& polyline, double dist, Point2D& position);

// scalar product of two vectors
double scalarProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2);

// Calculate the slope between two points.
double slope(double x1, double y1, double x2, double y2);

// vector product of two vectors
double vectorProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2);

// Read a file as a string.


#endif /* helper_hpp */
