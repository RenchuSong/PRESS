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

// WhereAt query on original PRESS trajectory.
void QueryProcessor::whereAt(
  const Graph& graph,
  const PRESSTrajectory& press,
  double timeStamp,
  Point2D& result
) {
  // Calculate d in temporal component at t.
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
