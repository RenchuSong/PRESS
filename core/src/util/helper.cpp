//
//  helper.cpp
//  press-v3
//
//  Created by Renchu Song on 6/21/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "helper.hpp"

// Convert degree to radian.
double deg2rad(double deg) {
  return deg * M_PI / 180;
}

// Distance of the a 2D point to a 2D interval.
double distPoint2Interval(const Point2D& point, const Point2D& end1, const Point2D& end2) {
  double result = std::min(euclideanDistance(point, end1), euclideanDistance(point, end2));
  if (scalarProduct(end1, point, end1, end2) > 0 && scalarProduct(end2, point, end2, end1) > 0) {
    result = std::min(
      result,
      fabs(vectorProduct(end1, end2, end1, point) / euclideanDistance(end1, end2))
    );
  }
  return result;
}

// Distance of the projection of the point to the geometry along the geometry.
double distProjAlongGeo(const Point2D& point, const std::vector<Point2D>& geometry) {
  double result = 0;
  double accum = 0;
  double minDist = 1e100;
  auto geoSize = geometry.size();
  for (auto i = 1; i < geoSize; i++) {
    auto dist = distPoint2Interval(point, geometry.at(i - 1), geometry.at(i));
    if (dist < minDist) {
      minDist = dist;
      result = accum + distProjAlongInterval(point, geometry.at(i - 1), geometry.at(i));
    }
    accum += euclideanDistance(geometry.at(i - 1), geometry.at(i));
  }
  return result;
}


// Distance of the projection of the point to the interval along the interval.
double distProjAlongInterval(const Point2D& point, const Point2D& end1, const Point2D& end2) {
  double edgeDist = euclideanDistance(end1, end2);
  return std::max(
    0.0,
    std::min(
      scalarProduct(end1, point, end1, end2) / edgeDist,
      edgeDist
    )
  );
}

// Euclidean distance between 2 points.
double euclideanDistance(const Point2D& p1, const Point2D& p2) {
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// Convert GPS point to 2D point.
Point2D gps2Point2D(const GPSPoint& gpsPoint) {
  // TODO: eliminate unnecessary copy.
  return Point2D(
    gpsPoint.longitude * M_PER_LONG * cos(deg2rad(gpsPoint.latitude)),
    gpsPoint.latitude * M_PER_LAT
  );
}

// TODO: maybe useful
//GPSPoint point2D2GPSPoint(double t, Point2D& point) {
//  return GPSPoint(
//    t,
//    point.y / M_PER_LAT,
//    point.x / M_PER_LONG / cos(deg2rad(point.y / M_PER_LAT))
//  );
//}

// Check if 2 intervals intersect with each other in strict case/
// Strict means touch doesn't count as intersect.
bool intervalIntersect(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& p3,
  const Point2D& p4
) {
  return vectorProduct(p1, p2, p1, p4) * vectorProduct(p1, p2, p1, p3) < 0
      && vectorProduct(p3, p4, p3, p1) * vectorProduct(p3, p4, p3, p2) < 0;
}

// Check if interval [p1, p2] goes through MBR [[minBound], [maxBound]] in strict case.
// Strict means touch doesn't count as goes through.
bool intervalThroughMBR(
  const Point2D& p1,
  const Point2D& p2,
  const Point2D& minBound,
  const Point2D& maxBound
) {
  Point2D p3(minBound.x, maxBound.y);
  Point2D p4(maxBound.x, minBound.y);
  return intervalIntersect(p1, p2, minBound, p3)
      || intervalIntersect(p1, p2, minBound, p4)
      || intervalIntersect(p1, p2, maxBound, p3)
      || intervalIntersect(p1, p2, maxBound, p4)
      || (pointOnInterval(minBound, p1, p2) && pointOnInterval(maxBound, p1, p2))
      || (pointOnInterval(p3, p1, p2) && pointOnInterval(p4, p1, p2));
}

// Check if a point is in an MBR.
bool pointInMBR(const Point2D& point, const Point2D& minBound, const Point2D& maxBound) {
  return point.x >= minBound.x && point.y >= minBound.y
      && point.x <= maxBound.x && point.y <= maxBound.y;
}

// Check if a point is on an interval.
bool pointOnInterval(const Point2D& point, const Point2D& p1, const Point2D& p2) {
  return fabs(vectorProduct(p1, point, p1, p2)) <= 1e-8
      && scalarProduct(point, p1, point, p2) <= 0;
}

// scalar product of two vectors
double scalarProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2) {
  double x1 = u2.x - u1.x;
  double y1 = u2.y - u1.y;
  double x2 = v2.x - v1.x;
  double y2 = v2.y - v1.y;
  
  return x1 * x2 + y1 * y2;
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

// vector product of two vectors
double vectorProduct(const Point2D& u1, const Point2D& u2, const Point2D& v1, const Point2D& v2) {
  double x1 = u2.x - u1.x;
  double y1 = u2.y - u1.y;
  double x2 = v2.x - v1.x;
  double y2 = v2.y - v1.y;
  
  return x1 * y2 - x2 * y1;
}
