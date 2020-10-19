//
//  query_processor.cpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "query_processor.hpp"
#include "../util/helper.hpp"

#include <cassert>

// Get how long distance the vehicle travelled along the trajectory at timeStamp.
double getTravelDistanceAtTimestamp(
  const Graph& graph,
  const PRESSTrajectory& press,
  double timeStamp
) {
  double d = -1;
  bool hasResult = false;
  auto& temporalComponent = press.getTemporalComponent();
  for (int i = 1; i < press.getTemporalLength(); ++i) {
    double t1 = temporalComponent.at(i - 1).t;
    double t2 = temporalComponent.at(i).t;
    if (t1 <= timeStamp && t2 >= timeStamp) {
      double d1 = temporalComponent.at(i - 1).dist;
      double d2 = temporalComponent.at(i).dist;
      d = linearInterpolate(t1, d1, t2, d2, timeStamp);
      hasResult = true;
      break;
    }
  }
  if (!hasResult) {
    throw "Timestamp is outside of trajectory travel period.";
  }
  return d;
}

// WhereAt query on original PRESS trajectory.
void QueryProcessor::whereAt(
  const Graph& graph,
  const PRESSTrajectory& press,
  double timeStamp,
  Point2D& result
) {
  // Calculate d in temporal component at t.
  double d = getTravelDistanceAtTimestamp(graph, press, timeStamp);

  // Get position from spatial component.
  for (int edgeId: press.getSpatialComponent()) {
    double len = graph.getEdge(edgeId).getDistance();
    if (len < d) {
      d -= len;
    } else {
      positionAlongPolyline(graph.getEdge(edgeId).getShape(), d, result);
      return;
    }
  }
}

// WhenAt query on original PRESS trajectory.
double QueryProcessor::whenAt(
  const Graph& graph,
  const PRESSTrajectory& press,
  const Point2D& position
) {
  double dist = 0;
  bool hasResult = false;
  for (int edgeId: press.getSpatialComponent()) {
    auto& edge = graph.getEdge(edgeId);
    auto& shape = edge.getShape();
    bool onEdge = false;
    for (int i = 1; i < shape.size(); ++i) {
      if (pointOnInterval(position, shape.at(i - 1), shape.at(i))) {
        onEdge = true;
        break;
      }
    }
    if (!onEdge) {
      dist += edge.getDistance();
    } else {
      dist += distProjAlongGeo(position, shape);
      hasResult = true;
      break;
    }
  }
  if (!hasResult) {
    throw "Trajectory never passed this position.";
  }

  auto& temporalComponent = press.getTemporalComponent();
  for (int i = 1; i < press.getTemporalLength(); ++i) {
    double d1 = temporalComponent.at(i - 1).dist;
    double d2 = temporalComponent.at(i).dist;
    if (d1 <= dist && d2 >= dist) {
      double t1 = temporalComponent.at(i - 1).t;
      double t2 = temporalComponent.at(i).t;
      return linearInterpolate(d1, t1, d2, t2, dist);
    }
  }

  throw "Internal error: should not be here.";
}

// Range query on original PRESS trajectory.
bool QueryProcessor::range(
  const Graph& graph,
  const PRESSTrajectory& press,
  double t1,
  double t2,
  const Point2D& lowerBound,
  const Point2D& upperBound
) {
  assert(t2 >= t1);
  // Travel distance range.
  double d1 = getTravelDistanceAtTimestamp(graph, press, t1);
  double d2 = getTravelDistanceAtTimestamp(graph, press, t2);

  double dist = 0;
  int state = 0; // 0: init; 1: cut from d1; 2: cut from d2.
  for (auto edgeId: press.getSpatialComponent()) {
    auto& edge = graph.getEdge(edgeId);
    // Skip edges before d1.
    if (dist + edge.getDistance() <= d1) {
      dist += edge.getDistance();
      continue;
    }
    // Precisely calculate shape.
    auto& shape = edge.getShape();
    for (int i = 1; i < shape.size(); ++i) {
      Point2D p1 = shape.at(i - 1);
      Point2D p2 = shape.at(i);
      double len = euclideanDistance(p1, p2);
      // Skip segments before d1.
      if (state == 0) {
        if (dist + len <= d1) {
          dist += len;
          continue;
        }
        // Cut to d1.
        state = 1;
        p1.setPosition(
          linearInterpolate(dist, p1.x, dist + len, p2.x, d1),
          linearInterpolate(dist, p1.y, dist + len, p2.y, d1)
        );
      }
      if (state == 1 && dist + len >= d2) {
        // Cut to d2.
        state = 2;
        p2.setPosition(
          linearInterpolate(dist, p1.x, dist + len, p2.x, d2),
          linearInterpolate(dist, p1.y, dist + len, p2.y, d2)
        );
      }
      // Check intersection.
      if (intervalThroughMBR(p1, p2, lowerBound, upperBound)) {
        return true;
      }
      if (state == 2) {
        return false;
      }
      dist += len;
    }
  }
  return false;
}
