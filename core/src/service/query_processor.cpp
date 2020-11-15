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
  const std::vector<TemporalPair>& temporalComponent,
  double timeStamp
) {
  double d = -1;
  bool hasResult = false;
  auto tempSize = temporalComponent.size();
  for (int i = 1; i < tempSize; ++i) {
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

// Get the 2D position along an edge sequence of distance dist.
void getPositionAlongEdgeSequence(
  const Graph& graph,
  const std::vector<int>& edgeSequence,
  double dist,
  Point2D& result
) {
  for (int edgeId: edgeSequence) {
    double len = graph.getEdge(edgeId).getDistance();
    if (len < dist) {
      dist -= len;
    } else {
      positionAlongPolyline(graph.getEdge(edgeId).getShape(), dist, result);
      return;
    }
  }
}

// Get timestamp of person traveled for dist on temporal component.
double getTimeOfTravelDistanceAtTemportalComponent(
  const std::vector<TemporalPair>& temporalComponent,
  double dist
) {
  auto len = temporalComponent.size();
  for (int i = 1; i < len; ++i) {
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

// Check if the position is on an edge, and update the distance.
bool positionOnEdgeWithDistance(
  const Graph& graph,
  int edgeId,
  const Point2D& position,
  double& dist
) {
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
    return false;
  } else {
    dist += distProjAlongGeo(position, shape);
    return true;
  }
}

// Check if the position is on an edge sequence, and update the distance.
bool positionOnEdgeSequenceWithDistance(
  const Graph& graph,
  const std::vector<int>& edgeSequence,
  const Point2D& position,
  double& dist
) {
  for (auto& edge: edgeSequence) {
    if (positionOnEdgeWithDistance(graph, edge, position, dist)) {
      return true;
    }
  }
  return false;
}

// WhereAt query on original PRESS trajectory.
void QueryProcessor::whereAt(
  const Graph& graph,
  const PRESSTrajectory& press,
  double timeStamp,
  Point2D& result
) {
  // Calculate d in temporal component at t.
  double d = getTravelDistanceAtTimestamp(graph, press.getTemporalComponent(), timeStamp);

  // Get position from spatial component.
  getPositionAlongEdgeSequence(graph, press.getSpatialComponent(), d, result);
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
    if (positionOnEdgeWithDistance(graph, edgeId, position, dist)) {
      hasResult = true;
      break;
    }
  }
  if (!hasResult) {
    throw "Trajectory never passed this position.";
  }

  return getTimeOfTravelDistanceAtTemportalComponent(press.getTemporalComponent(), dist);
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
  double d1 = getTravelDistanceAtTimestamp(graph, press.getTemporalComponent(), t1);
  double d2 = getTravelDistanceAtTimestamp(graph, press.getTemporalComponent(), t2);

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

// WhereAt query on compressed PRESS trajectory.
void QueryProcessor::whereAt(
  const Graph& graph,
  const SPTable& spTable,
  const Huffman& huffman,
  const ACAutomaton& acAutomaton,
  const Auxiliary& auxiliary,
  const PRESSCompressedTrajectory& press,
  double timeStamp,
  Point2D& result
) {
  // Calculate d in temporal component at t.
  double d = getTravelDistanceAtTimestamp(graph, press.getTemporalComponent(), timeStamp);
  // Get position.
  auto& binary = press.getSpatialComponent();
  int idx = 0;
  int preTrieNode = -1;
  int trieNode = -1;
  bool start = true;
  while ((trieNode = huffman.decodeNext(binary, idx)) != DECODE_FINISH) {
    // Check the gap between two trie nodes.
    if (!start) {
      int startNode = auxiliary.getTrieNodeEndNode(preTrieNode);
      int endNode = auxiliary.getTrieNodeStartNode(trieNode);
      double gapDist = auxiliary.getSPDistance(startNode, endNode);
      if (d <= gapDist) {
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        getPositionAlongEdgeSequence(graph, edgeSequence, d, result);
        return;
      }
      d -= gapDist;
    }
    start = false;
    preTrieNode = trieNode;
    // Skip the complete trie node.
    if (auxiliary.getTrieNodeDistance(trieNode) <= d) {
      d -= auxiliary.getTrieNodeDistance(trieNode);
      continue;
    }
    // Decode the trie node and find the exact position.
    std::vector<int> edges;
    int trieIdx = trieNode;
    while (trieIdx != ROOT_NODE) {
      edges.emplace_back(acAutomaton.getEdge(trieIdx));
      trieIdx = acAutomaton.getFather(trieIdx);
    }
    std::reverse(edges.begin(), edges.end());
    if (d <= graph.getEdge(edges.at(0)).getDistance()) {
      positionAlongPolyline(graph.getEdge(edges.at(0)).getShape(), d, result);
      return;
    }
    d -= graph.getEdge(edges.at(0)).getDistance();
    for (int i = 1; i < edges.size(); ++i) {
      int startNode = graph.getEdge(edges.at(i - 1)).getTargetId();
      int endNode = graph.getEdge(edges.at(i)).getSourceId();
      double gapDist = auxiliary.getSPDistance(startNode, endNode);
      if (d <= gapDist) {
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        getPositionAlongEdgeSequence(graph, edgeSequence, d, result);
        return;
      }
      d -= gapDist;
      if (d <= graph.getEdge(edges.at(i)).getDistance()) {
        positionAlongPolyline(graph.getEdge(edges.at(i)).getShape(), d, result);
        return;
      }
      d -= graph.getEdge(edges.at(i)).getDistance();
    }
  }
}

// WhenAt query on compressed PRESS trajectory.
double QueryProcessor::whenAt(
  const Graph& graph,
  const SPTable& spTable,
  const Huffman& huffman,
  const ACAutomaton& acAutomaton,
  const Auxiliary& auxiliary,
  const PRESSCompressedTrajectory& press,
  const Point2D& position
) {
  double dist = 0;
  bool hasResult = false;
  auto& binary = press.getSpatialComponent();
  int idx = 0;
  int preTrieNode = -1;
  int trieNode = -1;
  bool start = true;
  while ((trieNode = huffman.decodeNext(binary, idx)) != DECODE_FINISH) {
    // Check the gap between two trie nodes.
    if (!start) {
      int startNode = auxiliary.getTrieNodeEndNode(preTrieNode);
      int endNode = auxiliary.getTrieNodeStartNode(trieNode);
      const auto& mbr = auxiliary.getNodePairMBR(graph, startNode, endNode);
      if (!pointInMBR(position, mbr.first, mbr.second)) {
        // Skip the gap
        dist += auxiliary.getSPDistance(startNode, endNode);
      } else {
        // Check the edge sequence.
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        if (positionOnEdgeSequenceWithDistance(graph, edgeSequence, position, dist)) {
          hasResult = true;
          break;
        }
      }
    }
    start = false;
    preTrieNode = trieNode;
    // Skip the complete trie node.
    auto& mbr = auxiliary.getTrieNodeMBR(trieNode);
    if (!pointInMBR(position, mbr.first, mbr.second)) {
      dist += auxiliary.getTrieNodeDistance(trieNode);
      continue;
    }
    // Decode the trie node and find the exact distance.
    std::vector<int> edges;
    int trieIdx = trieNode;
    while (trieIdx != ROOT_NODE) {
      edges.emplace_back(acAutomaton.getEdge(trieIdx));
      trieIdx = acAutomaton.getFather(trieIdx);
    }
    std::reverse(edges.begin(), edges.end());
    if (positionOnEdgeWithDistance(graph, edges.at(0), position, dist)) {
      hasResult = true;
      break;
    }
    for (int i = 1; i < edges.size(); ++i) {
      int startNode = graph.getEdge(edges.at(i - 1)).getTargetId();
      int endNode = graph.getEdge(edges.at(i)).getSourceId();
      const auto& mbr = auxiliary.getNodePairMBR(graph, startNode, endNode);
      if (!pointInMBR(position, mbr.first, mbr.second)) {
        dist += auxiliary.getSPDistance(startNode, endNode);
      } else {
        // Check the edge sequence.
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        if (positionOnEdgeSequenceWithDistance(graph, edgeSequence, position, dist)) {
          hasResult = true;
          break;
        }
      }
      if (positionOnEdgeWithDistance(graph, edges.at(i), position, dist)) {
        hasResult = true;
        break;
      }
    }
    if (hasResult) {
      break;
    }
  }
  if (!hasResult) {
    throw "Trajectory never passed this position.";
  }

  return getTimeOfTravelDistanceAtTemportalComponent(press.getTemporalComponent(), dist);
}
