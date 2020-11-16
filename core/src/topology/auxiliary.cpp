//
//  auxiliary.cpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>

#include "auxiliary.hpp"
#include "../util/helper.hpp"

void findDistance(
  const Graph& graph,
  const std::unordered_map<int, int>& targetPrev,
  std::unordered_map<int, double>& targetSPDist,
  int src,
  int node
) {
  if (targetSPDist.find(node) != targetSPDist.end() || src == node) {
    return;
  }
  std::vector<int> edges;
  while (targetSPDist.find(node) == targetSPDist.end() && node != src) {
    if (targetPrev.find(node) == targetPrev.end()) {
      break;
    }
    int edge = targetPrev.at(node);
    if (edge == EDGE_NOT_EXIST) {
      break;
    }
    edges.emplace_back(edge);
    node = graph.getEdge(edge).getSourceId();
  }
  if (edges.size() > 0) {
    std::reverse(edges.begin(), edges.end());
    double dist = 0;
    auto& firstEdge = graph.getEdge(edges.at(0));
    if (targetSPDist.find(firstEdge.getSourceId()) != targetSPDist.end()) {
      dist = targetSPDist.at(firstEdge.getSourceId());
    }
    for (int eid: edges) {
      auto& edge = graph.getEdge(eid);
      dist += edge.getDistance();
      targetSPDist[edge.getTargetId()] = dist;
    }
  }
  return;
}

void findMBR(
  const Graph& graph,
  const std::unordered_map<int, int>& targetPrev,
  std::unordered_map<int, std::pair<Point2D, Point2D> >& targetMBR,
  int src,
  int node,
  const std::vector<std::pair<Point2D, Point2D> >& edgeMBR
) {
  if (targetMBR.find(node) != targetMBR.end() || src == node) {
    return;
  }
  std::vector<int> edges;
  while (targetMBR.find(node) == targetMBR.end() && node != src) {
    if (targetPrev.find(node) == targetPrev.end()) {
      break;
    }
    int edge = targetPrev.at(node);
    if (edge == EDGE_NOT_EXIST) {
      break;
    }
    edges.emplace_back(edge);
    node = graph.getEdge(edge).getSourceId();
  }
  if (edges.size() > 0) {
    std::reverse(edges.begin(), edges.end());
    auto mbr = std::make_pair(graph.getNode(src).getPosition(), graph.getNode(src).getPosition());
    auto& firstEdge = graph.getEdge(edges.at(0));
    if (targetMBR.find(firstEdge.getSourceId()) != targetMBR.end()) {
      mbr = targetMBR.at(firstEdge.getSourceId());
    }
    for (int eid: edges) {
      if (eid >= edgeMBR.size()) {
        std::cout << eid << " " << edgeMBR.size() << std::endl;
      }
      auto& edge = graph.getEdge(eid);
      extendMBR(mbr, edgeMBR.at(eid).first);
      extendMBR(mbr, edgeMBR.at(eid).second);
      targetMBR[edge.getTargetId()] = mbr;
    }
  }
  return;
}

Auxiliary::Auxiliary(const Graph& graph, const SPTable& spTable, const ACAutomaton& acAutomaton) {
  // Shortest path distance.
  nodeNumber = spTable.nodeNumber;
  for (int i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, double> targetSPDist;
    for (auto& nodePair: spTable.prevEdge[i]) {
      findDistance(graph, spTable.prevEdge[i], targetSPDist, i, nodePair.first);
    }
    nodePairSPDist.emplace_back(targetSPDist);
  }
  // Trie node distance, start node and end node.
  trieSize = acAutomaton.getTrieSize();
  trieNodeDist.emplace_back(0);
  trieNodeStartNode.emplace_back(-1);
  trieNodeEndNode.emplace_back(-1);
  for (int i = 1; i < trieSize; ++i) {
    int father = acAutomaton.getFather(i);
    auto& fatherEdge = graph.getEdge(acAutomaton.getEdge(father));
    auto& edge = graph.getEdge(acAutomaton.getEdge(i));
    trieNodeDist.emplace_back(
      trieNodeDist.at(father) +
      edge.getDistance() +
      getSPDistance(fatherEdge.getTargetId(), edge.getSourceId())
    );
    if (father == ROOT_NODE) {
      trieNodeStartNode.emplace_back(edge.getSourceId());
    } else {
      trieNodeStartNode.emplace_back(trieNodeStartNode.at(father));
    }
    trieNodeEndNode.emplace_back(edge.getTargetId());
  }
  // MBR of each edge in the graph.
  edgeNumber = graph.getEdgeNumber();
  for (int i = 0; i < edgeNumber; ++i) {
    auto& shape = graph.getEdge(i).getShape();
    auto mbr = std::make_pair(Point2D(shape.at(0)), Point2D(shape.at(0)));
    for (auto& point: shape) {
      extendMBR(mbr, point);
    }
    edgeMBR.emplace_back(mbr);
  }
  // MBR of shortest path between each pair of nodes.
  for (int i = 0; i < nodeNumber; ++i) {
    std::unordered_map<int, std::pair<Point2D, Point2D> > targetMBR;
    for (auto& nodePair: spTable.prevEdge[i]) {
      findMBR(graph, spTable.prevEdge[i], targetMBR, i, nodePair.first, edgeMBR);
    }
    nodePairMBR.emplace_back(targetMBR);
  }
  // MBR of each Trie node.
  trieNodeMBR.emplace_back(std::make_pair(Point2D(0, 0), Point2D(0, 0)));
  for (int i = 1; i < trieSize; ++i) {
    int father = acAutomaton.getFather(i);
    auto& fatherEdge = graph.getEdge(acAutomaton.getEdge(father));
    auto& edge = graph.getEdge(acAutomaton.getEdge(i));
    auto mbr = edgeMBR.at(acAutomaton.getEdge(i));
    if (father != ROOT_NODE) {
      // Extend the father
      extendMBR(mbr, trieNodeMBR.at(father).first);
      extendMBR(mbr, trieNodeMBR.at(father).second);
      // Extend the node pair.
      auto npMbr = getNodePairMBR(graph, fatherEdge.getTargetId(), edge.getSourceId());
      extendMBR(mbr, npMbr.first);
      extendMBR(mbr, npMbr.second);
    }
    trieNodeMBR.emplace_back(mbr);
  }
}

Auxiliary::Auxiliary(FileReader& auxReader) {
  // TODO
}

void Auxiliary::store(FileWriter& auxWriter) {
  // TODO
}

// Get the node number.
size_t Auxiliary::getNodeNumber() const {
  return nodeNumber;
}

// Get the shortest path distance between two nodes.
double Auxiliary::getSPDistance(size_t srcIndex, size_t tgtIndex) const {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  auto& pairDist = nodePairSPDist.at(srcIndex);
  if (pairDist.find((int)tgtIndex) != pairDist.end()) {
    return pairDist.at((int)tgtIndex);
  } else {
    // This means the sequence is not consecutive, in this case treat as new trajectory segment.
    return 0;
  }
}

// Get the trie node distance.
double Auxiliary::getTrieNodeDistance(size_t index) const {
  assert(index < trieSize);
  return trieNodeDist.at(index);
}

// Get the trie node start node.
int Auxiliary::getTrieNodeStartNode(size_t index) const {
  assert(index < trieSize);
  return trieNodeStartNode.at(index);
}

// Get the trie node end node.
int Auxiliary::getTrieNodeEndNode(size_t index) const {
  assert(index < trieSize);
  return trieNodeEndNode.at(index);
}

// Get the shortest path distance between two nodes.
std::pair<Point2D, Point2D> Auxiliary::getNodePairMBR(
  const Graph& graph,
  size_t srcIndex,
  size_t tgtIndex
) const {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  auto& pairMBR = nodePairMBR.at(srcIndex);
  if (pairMBR.find((int)tgtIndex) != pairMBR.end()) {
    return pairMBR.at((int)tgtIndex);
  } else {
    // This means there is no data recorded.
    return std::make_pair(
      graph.getNode(srcIndex).getPosition(),
      graph.getNode(tgtIndex).getPosition()
    );
  }
}

// Get the trie node MBR.
const std::pair<Point2D, Point2D>& Auxiliary::getTrieNodeMBR(size_t index) const {
  assert(index < trieSize);
  return trieNodeMBR.at(index);
}

// Get the edge MBR.
const std::pair<Point2D, Point2D>& Auxiliary::getEdgeMBR(size_t index) const {
  assert(index < edgeNumber);
  return edgeMBR.at(index);
}

void Auxiliary::print() const {
  // TODO
}

Auxiliary::~Auxiliary() { }
