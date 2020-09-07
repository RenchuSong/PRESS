//
//  graph_reader_base.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "graph_reader_base.hpp"

#include <cassert>

#include "../../util/helper.hpp"

void GraphReaderBase::readGraph(const std::string& fileName, Graph& graph) {
  readGraphImpl(fileName, graph);
  sanityCheck(graph);
}

void GraphReaderBase::sanityCheck(Graph& graph) const {
  auto& nodeList = graph.getNodeList();
  auto& edgeList = graph.getEdgeList();
  
  // 1. Graph cannot be empty.
  assert(graph.getNodeNumber() > 0);
  assert(graph.getEdgeNumber() > 0);

  // 1. All latitudes and longitudes fall in reasonable range.
  GPSPoint minLatGPS(-90, 0);
  auto minBoundY = gps2Point2D(minLatGPS);
  GPSPoint minLongGPS(0, -180);
  auto minBoundX = gps2Point2D(minLongGPS);
  GPSPoint maxLatGPS(90, 0);
  auto maxBoundY = gps2Point2D(maxLatGPS);
  GPSPoint maxLongGPS(0, 180);
  auto maxBoundX = gps2Point2D(maxLongGPS);
  Point2D minBound(minBoundX.x, minBoundY.y);
  Point2D maxBound(maxBoundX.x, maxBoundY.y);
  for (auto node: nodeList) {
    assert(pointInMBR(node.getPosition(), minBound, maxBound));
  }
  for (auto edge: edgeList) {
    for (auto geoPos: edge.getShape()) {
      assert(pointInMBR(geoPos, minBound, maxBound));
    }
  }

  // 2. No edge is connected by non-existing nodes.
  for (auto edge: edgeList) {
    assert(edge.getSourceId() >= 0 && edge.getSourceId() < nodeList.size());
    assert(edge.getTargetId() >= 0 && edge.getTargetId() < nodeList.size());
  }
  
  // 3. Edge start/end position should be same as source/target node position.
  for (auto edge: edgeList) {
    auto edgeShape = edge.getShape();
    assert(euclideanDistance(edgeShape.at(0), nodeList.at(edge.getSourceId()).getPosition()) < 1);
    assert(
      euclideanDistance(
        edgeShape.at(edgeShape.size() - 1),
        nodeList.at(edge.getTargetId()).getPosition()
      ) < 1
    );
  }
}
