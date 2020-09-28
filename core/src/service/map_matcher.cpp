//
//  map_matcher.cpp
//  press-v3
//
//  Created by Renchu Song on 9/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "map_matcher.hpp"
#include "../util/helper.hpp"

// Measurement probability
double measurementProb(const Point2D& gpsPoint, const Edge& edge, double sigmaZ) {
  double sqrt2PISigmaZ = 1 / sqrt(2 * M_PI) / sigmaZ;
  double exponent = -0.5 * pow(distPoint2Polyline(gpsPoint, edge.getShape()) / sigmaZ, 2);
  return sqrt2PISigmaZ * exp(exponent);
}

// Calculate differenece between great circle distance and route distance.
double distanceDifference(
  const Point2D& p1,
  int edge1,
  const Point2D& p2,
  int edge2,
  const Graph& graph,
  const SPTable& spTable
) {
  double d1 = euclideanDistance(p1, p2);
  double d2;
  if (edge1 == edge2) {
    auto& shape = graph.getEdge(edge1).getShape();
    d2 = fabs(distProjAlongGeo(p1, shape) - distProjAlongGeo(p2, shape));
  } else {
    std::vector<int> edgeSequence;
    spTable.complement(graph, edge1, edge2, edgeSequence);
    auto& edge1Entity = graph.getEdge(edge1);
    auto& edge2Entity = graph.getEdge(edge2);
    auto& shape1 = edge1Entity.getShape();
    auto& shape2 = edge2Entity.getShape();
    d2 = edge1Entity.getDistance() - distProjAlongGeo(p1, shape1) + distProjAlongGeo(p2, shape2);
    int seqSize = (int)edgeSequence.size() - 1;
    for (int i = 0; i < seqSize; ++i) {
      d2 += graph.getEdge(edgeSequence.at(i)).getDistance();
    }
  }
  return fabs(d1 - d2);
}

// Transition probability
double transitionProb(
  const Point2D& prevPoint,
  int prevEdge,
  const Point2D& nextPoint,
  int nextEdge,
  const Graph& graph,
  const SPTable& spTable,
  double beta,
  double maxDistDifference
) {
  double diff = distanceDifference(prevPoint, prevEdge, nextPoint, nextEdge, graph, spTable);
  return diff < maxDistDifference ? (1 / beta * exp(-diff / beta)) : 0;
}

void MapMatcher::mapMatch(
  const SPTable& spTable,
  const Graph& graph,
  const GridIndex& gridIndex,
  double sigmaZ,
  double beta,
  double maxGPSBias,
  double maxDistDifference,
  const GPSTrajectory& gpsTrajectory,
  std::vector<GPSTrajectory>& gpsTrajectories,
  std::vector<MapMatchedTrajectory>& mmTrajectories
) const {
  // The previous matching edge.
  std::vector<std::unordered_map<int, int> > prev;
  // Matching probability of each GPS point to each edge.
  std::vector<std::unordered_map<int, double> > prob;
  
  std::cout << gpsTrajectory.getLength() << std::endl;

  // Viterbi algorithm for HMM.
  for (int i = 0; i < gpsTrajectory.getLength(); ++i) {
    if (i % 1000 == 0) {
      std::cout << i << std::endl;
    }
    auto& gpsPoint1 = gpsTrajectory.getGPSPoint(i);
    Point2D p1 = gps2Point2D(gpsPoint1);
    std::unordered_set<int> nearByEdges;
    gridIndex.search(p1, maxGPSBias, nearByEdges);
    std::unordered_map<int, int> pointPrev;
    std::unordered_map<int, double> pointProb;
    for (int eid: nearByEdges) {
      double measureProb = measurementProb(p1, graph.getEdge(eid), sigmaZ);
      if (i == 0) {
        pointPrev[eid] = EDGE_NOT_EXIST;
        pointProb[eid] = measureProb;
      } else {
        auto& gpsPoint2 = gpsTrajectory.getGPSPoint(i - 1);
        Point2D p2 = gps2Point2D(gpsPoint2);
        auto& prevProb = prob.at(i - 1);
        double maxProb = 0;
        int prevEdge = EDGE_NOT_EXIST;
        for (auto& edgeProbPair: prevProb) {
          double accumProb = edgeProbPair.second * transitionProb(
            p2, edgeProbPair.first, p1, eid, graph, spTable, beta, maxDistDifference
          );
          if (accumProb > maxProb) {
            maxProb = accumProb;
            prevEdge = edgeProbPair.first;
          }
        }
        pointPrev[eid] = prevEdge;
        pointProb[eid] = maxProb * measureProb;
      }
    }
    // Normalize the probability.
    double maxProb = 0;
    for (auto& eidProdPair: pointProb) {
      maxProb = std::max(maxProb, eidProdPair.second);
    }
    if (maxProb > 0) {
      for (auto& eidProdPair: pointProb) {
        pointProb[eidProdPair.first] /= maxProb;
      }
    }
    prev.emplace_back(pointPrev);
    prob.emplace_back(pointProb);
  }

  // Collect map matching result.
  std::vector<GPSPoint> gpsTrajCollector;
  std::vector<int> mmResultCollector;
  int prevEdge = EDGE_NOT_EXIST;
  for (int i = (int)gpsTrajectory.getLength() - 1; i >= 0; --i) {
    // Matching is broken, choose the highest probability one.
    if (prevEdge == EDGE_NOT_EXIST) {
      double maxProb = 0;
      auto& matchedEdges = prob.at(i);
      for (auto& edgeProbPair: matchedEdges) {
        if (edgeProbPair.second > maxProb) {
          maxProb = edgeProbPair.second;
          prevEdge = edgeProbPair.first;
        }
      }
    }
    // Collect
    gpsTrajCollector.emplace_back(gpsTrajectory.getGPSPoint(i));
    mmResultCollector.emplace_back(prevEdge);
    // Move backward. If broken, append to result.
    if (prevEdge != EDGE_NOT_EXIST) {
      prevEdge = prev.at(i).at(prevEdge);
    }
    if (prevEdge == EDGE_NOT_EXIST) {
      std::reverse(gpsTrajCollector.begin(), gpsTrajCollector.end());
      std::reverse(mmResultCollector.begin(), mmResultCollector.end());
      gpsTrajectories.emplace_back(gpsTrajCollector);
      mmTrajectories.emplace_back(mmResultCollector);
      gpsTrajCollector.clear();
      mmResultCollector.clear();
    }
  }
  for (auto& pointProb: prob) {
    double maxProb = 0;
    for (auto& p: pointProb) {
      maxProb = std::max(maxProb, p.second);
    }
    std::cout << maxProb << std::endl;
  }
}
