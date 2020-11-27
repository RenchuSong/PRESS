//
//  map_matcher.cpp
//  press-v3
//
//  Created by Renchu Song on 9/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <algorithm>

#include "map_matcher.hpp"
#include "../util/helper.hpp"

// https://dl.acm.org/doi/abs/10.1145/2424321.2424428
const double HMM_TRANSITION_BETA[30] = {
  0.49037673,
  0.82918373,
  1.24364564,
  1.67079581,
  2.00719298,
  2.42513007,
  2.81248831,
  3.15745473,
  3.52645392,
  4.09511775,
  4.67319795,
  5.41088180,
  6.47666590,
  6.29010734,
  7.80752112,
  8.09074504,
  8.08550528,
  9.09405065,
  11.09090603,
  11.87752824,
  12.55107715,
  15.82820829,
  17.69496773,
  18.07655652,
  19.63438911,
  25.40832185,
  23.76001877,
  28.43289797,
  32.21683062,
  34.56991141
};
double getBeta(const GPSPoint& prev, const GPSPoint& next) {
  return HMM_TRANSITION_BETA[
    std::min(std::max((int)round(next.t - prev.t - 1), 0), 29)
  ];
}

// Measurement probability
double measurementProb(const Point2D& gpsPoint, const Edge& edge, double sigmaZ) {
//  double sqrt2PISigmaZ = 1 / sqrt(2 * M_PI) / sigmaZ;
  double exponent = -0.5 * pow(distPoint2Polyline(gpsPoint, edge.getShape()) / sigmaZ, 2);
  return /*sqrt2PISigmaZ * */ exp(exponent);
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
  return diff < maxDistDifference ? (/*1 / beta * */ exp(-diff / beta)) : 0;
}

void MapMatcher::mapMatch(
  const SPTable& spTable,
  const Graph& graph,
  const GridIndex& gridIndex,
  double sigmaZ,
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

  // Viterbi algorithm for HMM.
  for (int i = 0; i < gpsTrajectory.getLength(); ++i) {
    auto& gpsPoint1 = gpsTrajectory.getGPSPoint(i);
    Point2D p1 = gps2Point2D(gpsPoint1);
    std::unordered_set<int> nearByEdges;
    gridIndex.search(p1, maxGPSBias, nearByEdges);
    std::unordered_map<int, int> pointPrev;
    std::unordered_map<int, double> pointProb;
    std::unordered_map<int, double> measureProbs;

    for (int eid: nearByEdges) {
      double measureProb = measurementProb(p1, graph.getEdge(eid), sigmaZ);
      pointPrev[eid] = EDGE_NOT_EXIST;
      measureProbs[eid] = measureProb;
    }
    if (i == 0) {
      pointProb = measureProbs;
    } else {
      double beta = getBeta(gpsTrajectory.getGPSPoint(i - 1), gpsTrajectory.getGPSPoint(i));
      double maxAllProb = 0;
      for (int eid: nearByEdges) {
        auto& gpsPoint2 = gpsTrajectory.getGPSPoint(i - 1);
        Point2D p2 = gps2Point2D(gpsPoint2);
        auto& prevProb = prob.at(i - 1);
        double maxProb = 0;
        int prevEdge = EDGE_NOT_EXIST;
        for (auto& edgeProbPair: prevProb) {
          if (edgeProbPair.second == 0) {
            continue;
          }
          double accumProb = edgeProbPair.second * transitionProb(
            p2, edgeProbPair.first, p1, eid, graph, spTable, beta, maxDistDifference
          );
          if (accumProb > maxProb) {
            maxProb = accumProb;
            prevEdge = edgeProbPair.first;
          }
        }
        maxAllProb = std::max(maxAllProb, maxProb);
        pointPrev[eid] = prevEdge;
        pointProb[eid] = maxProb * measureProbs[eid];
      }
      if (maxAllProb == 0) {
        pointProb = measureProbs;
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
        if (edgeProbPair.second >= maxProb) {
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
      // Trajectory reformatter is in charge of dedup & complementing the edge sequence.
//      std::vector<int> edgeSequence;
//      edgeSequence.emplace_back(mmResultCollector.front());
//      auto len = mmResultCollector.size();
//      for (int j = 1; j < len; ++j) {
//        if (mmResultCollector.at(j - 1) != mmResultCollector.at(j)) {
//          spTable.complement(
//            graph,
//            mmResultCollector.at(j - 1),
//            mmResultCollector.at(j),
//            edgeSequence
//          );
//        }
//      }
      gpsTrajectories.emplace_back(GPSTrajectory(gpsTrajCollector));
      mmTrajectories.emplace_back(MapMatchedTrajectory(mmResultCollector));
      gpsTrajCollector.clear();
      mmResultCollector.clear();
    }
  }
  for (auto& pointProb: prob) {
    double maxProb = 0;
    for (auto& p: pointProb) {
      maxProb = std::max(maxProb, p.second);
    }
  }
}
