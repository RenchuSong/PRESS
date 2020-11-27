//
//  query_processor.cpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <algorithm>

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

// Get the intersection of shape and distance range [d1, d2].
std::vector<Point2D> getShapeDistRangeIntersection(
  const std::vector<Point2D>& shape,
  double dist,
  double upperDist,
  double d1,
  double d2
) {
  d1 = std::max(d1, dist);
  d2 = std::min(d2, upperDist);
  std::vector<Point2D> result;
  for (int i = 1; i < shape.size(); ++i) {
    Point2D p1 = shape.at(i - 1);
    Point2D p2 = shape.at(i);
    double len = euclideanDistance(p1, p2);
    // Skip segments before d1.
    if (dist + len <= d1) {
      dist += len;
      continue;
    }
    // Cut to d1.
    p1.setPosition(
      linearInterpolate(dist, p1.x, dist + len, p2.x, d1),
      linearInterpolate(dist, p1.y, dist + len, p2.y, d1)
    );
    result.emplace_back(p1);
    if (dist + len >= d2) {
      // Cut to d2.
      p2.setPosition(
        linearInterpolate(dist, p1.x, dist + len, p2.x, d2),
        linearInterpolate(dist, p1.y, dist + len, p2.y, d2)
      );
      result.emplace_back(p2);
      break;
    }
    dist += len;
  }
  return result;
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

// Check MBR intersect with shape.
bool checkMBRInstersectWithShape(
  const Point2D& lowerBound,
  const Point2D& upperBound,
  const std::vector<Point2D>& shape
) {
  auto len = shape.size();
  for (int i = 1; i < len; ++i) {
    if (intervalThroughMBR(shape.at(i - 1), shape.at(i), lowerBound, upperBound)) {
      return true;
    }
  }
  return false;
}

// Check MBR intersect with edge.
bool checkMBRInstersectWithEdge(
  const Graph& graph,
  const Auxiliary& auxiliary,
  const Point2D& lowerBound,
  const Point2D& upperBound,
  int edgeId
) {
  auto& mbr = auxiliary.getEdgeMBR(edgeId);
  if (!mbrIntersectWithMBR(lowerBound, upperBound, mbr.first, mbr.second)) {
    return false;
  }
  auto& shape = graph.getEdge(edgeId).getShape();
  if (checkMBRInstersectWithShape(lowerBound, upperBound, shape)) {
    return true;
  }
  return false;
}

// Check MBR intersect with SP pair.
bool checkMBRInstersectWithSPPair(
  const Graph& graph,
  const SPTable& spTable,
  const Auxiliary& auxiliary,
  const Point2D& lowerBound,
  const Point2D& upperBound,
  int startNode,
  int endNode
) {
  const auto& mbr = auxiliary.getNodePairMBR(graph, startNode, endNode);
  if (!mbrIntersectWithMBR(lowerBound, upperBound, mbr.first, mbr.second)) {
    return false;
  }
  std::vector<int> edgeSequence;
  spTable.complementNode(graph, startNode, endNode, edgeSequence);
  for (auto& edgeId: edgeSequence) {
    if (checkMBRInstersectWithEdge(graph, auxiliary, lowerBound, upperBound, edgeId)) {
      return true;
    }
  }
  return false;
}

// Range query on compressed PRESS trajectory.
bool QueryProcessor::range(
  const Graph& graph,
  const SPTable& spTable,
  const Huffman& huffman,
  const ACAutomaton& acAutomaton,
  const Auxiliary& auxiliary,
  const PRESSCompressedTrajectory& press,
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
  // The complete trie nodes to check MBR intersection.
  std::vector<int> trieNodesToCheck;
  // The complete SP pairs to check MBR intersection.
  std::vector<std::pair<int, int> > spPairsToCheck;
  // The edges to check MBR intersection.
  std::vector<int> edgesToCheck;
  // The shapes need to check MBR intersection.
  std::vector<std::vector<Point2D> > shapesToCheck;

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
      if (dist + gapDist <= d1) {
        // Skip the whole SP pair.
        dist += gapDist;
      } else if (dist >= d1 && dist + gapDist <= d2) {
        // Add the whole SP pair.
        spPairsToCheck.emplace_back(std::make_pair(startNode, endNode));
        dist += gapDist;
      } else {
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        for (auto& edgeId: edgeSequence) {
          auto& edge = graph.getEdge(edgeId);
          // Skip the edge
          if (dist + edge.getDistance() <= d1) {
            dist += edge.getDistance();
            continue;
          }
          // Add the whole edge for check.
          if (dist >= d1 && dist + edge.getDistance() <= d2) {
            edgesToCheck.emplace_back(edgeId);
            dist += edge.getDistance();
            continue;
          }
          // Add shape for check.
          shapesToCheck.emplace_back(
            getShapeDistRangeIntersection(
              edge.getShape(),
              dist,
              dist + edge.getDistance(),
              d1,
              d2
            )
          );
          dist += edge.getDistance();
          if (dist >= d2) {
            break;
          }
        }
      }
      if (dist >= d2) {
        break;
      }
    }
    start = false;
    preTrieNode = trieNode;
    // Skip the complete trie node.
    double trieDist = auxiliary.getTrieNodeDistance(trieNode);
    if (dist + trieDist <= d1) {
      dist += trieDist;
      continue;
    }
    // Add the whole trie node for check.
    if (dist >= d1 && dist + trieDist <= d2) {
      dist += trieDist;
      trieNodesToCheck.emplace_back(trieNode);
      continue;
    }
    // Decode the trie node and find the exact stuff to check.
    std::vector<int> edges;
    int trieIdx = trieNode;
    while (trieIdx != ROOT_NODE) {
      edges.emplace_back(acAutomaton.getEdge(trieIdx));
      trieIdx = acAutomaton.getFather(trieIdx);
    }
    std::reverse(edges.begin(), edges.end());
    // For 1st edge.
    auto& edge = graph.getEdge(edges.at(0));
    if (dist + edge.getDistance() <= d1) {
      // Skip the 1st edge
      dist += edge.getDistance();
    } else if (dist >= d1 && dist + edge.getDistance() <= d2) {
      // Add the whole 1st edge for check.
      edgesToCheck.emplace_back(edges.at(0));
      dist += edge.getDistance();
    } else {
      // Add shape of 1st edge for check.
      shapesToCheck.emplace_back(
        getShapeDistRangeIntersection(
          edge.getShape(),
          dist,
          dist + edge.getDistance(),
          d1,
          d2
        )
      );
      dist += edge.getDistance();
      if (dist >= d2) {
        break;
      }
    }
    // For following SP and edges.
    for (int i = 1; i < edges.size(); ++i) {
      int startNode = graph.getEdge(edges.at(i - 1)).getTargetId();
      int endNode = graph.getEdge(edges.at(i)).getSourceId();
      // Check the gap.
      double gapDist = auxiliary.getSPDistance(startNode, endNode);
      if (dist + gapDist <= d1) {
        // Skip the whole SP pair.
        dist += gapDist;
      } else if (dist >= d1 && dist + gapDist <= d2) {
        // Add the whole SP pair.
        spPairsToCheck.emplace_back(std::make_pair(startNode, endNode));
        dist += gapDist;
      } else {
        std::vector<int> edgeSequence;
        spTable.complementNode(graph, startNode, endNode, edgeSequence);
        for (auto& edgeId: edgeSequence) {
          auto& edge = graph.getEdge(edgeId);
          // Skip the edge
          if (dist + edge.getDistance() <= d1) {
            dist += edge.getDistance();
            continue;
          }
          // Add the whole edge for check.
          if (dist >= d1 && dist + edge.getDistance() <= d2) {
            edgesToCheck.emplace_back(edgeId);
            dist += edge.getDistance();
            continue;
          }
          // Add shape for check.
          shapesToCheck.emplace_back(
            getShapeDistRangeIntersection(
              edge.getShape(),
              dist,
              dist + edge.getDistance(),
              d1,
              d2
            )
          );
          dist += edge.getDistance();
          if (dist >= d2) {
            break;
          }
        }
      }
      if (dist >= d2) {
        break;
      }
      // Check the edge.
      auto& edge = graph.getEdge(edges.at(i));
      if (dist + edge.getDistance() <= d1) {
        // Skip the edge
        dist += edge.getDistance();
      } else if (dist >= d1 && dist + edge.getDistance() <= d2) {
        // Add the whole edge for check.
        edgesToCheck.emplace_back(edges.at(i));
        dist += edge.getDistance();
      } else {
        // Add shape of edge for check.
        shapesToCheck.emplace_back(
          getShapeDistRangeIntersection(
            edge.getShape(),
            dist,
            dist + edge.getDistance(),
            d1,
            d2
          )
        );
        dist += edge.getDistance();
        if (dist >= d2) {
          break;
        }
      }
    }

    if (dist >= d2) {
      break;
    }
  }

  // Check intersection.
  // For shapes.
  for (auto& shape: shapesToCheck) {
    if (checkMBRInstersectWithShape(lowerBound, upperBound, shape)) {
      return true;
    }
  }
  // For edges.
  for (auto& edgeId: edgesToCheck) {
    if (checkMBRInstersectWithEdge(graph, auxiliary, lowerBound, upperBound, edgeId)) {
      return true;
    }
  }
  // For SP pairs.
  for (auto& spPair: spPairsToCheck) {
    if (
      checkMBRInstersectWithSPPair(
        graph,
        spTable,
        auxiliary,
        lowerBound,
        upperBound,
        spPair.first,
        spPair.second
      )
    ) {
      return true;
    }
  }
  // For Trie Nodes.
  for (auto trieNode: trieNodesToCheck) {
    auto& mbr = auxiliary.getTrieNodeMBR(trieNode);
    if (!mbrIntersectWithMBR(lowerBound, upperBound, mbr.first, mbr.second)) {
      continue;
    }
    // Decode the trie node and find the exact stuff to check.
    std::vector<int> edges;
    int trieIdx = trieNode;
    while (trieIdx != ROOT_NODE) {
      edges.emplace_back(acAutomaton.getEdge(trieIdx));
      trieIdx = acAutomaton.getFather(trieIdx);
    }
    std::reverse(edges.begin(), edges.end());
    if (checkMBRInstersectWithEdge(graph, auxiliary, lowerBound, upperBound, edges.at(0))) {
      return true;
    }
    for (int i = 1; i < edges.size(); ++i) {
      int startNode = graph.getEdge(edges.at(i - 1)).getTargetId();
      int endNode = graph.getEdge(edges.at(i)).getSourceId();
      if (
        checkMBRInstersectWithSPPair(
          graph,
          spTable,
          auxiliary,
          lowerBound,
          upperBound,
          startNode,
          endNode
        )
      ) {
        return true;
      }
      if (checkMBRInstersectWithEdge(graph, auxiliary, lowerBound, upperBound, edges.at(i))) {
        return true;
      }
    }
  }
  
  return false;
}


