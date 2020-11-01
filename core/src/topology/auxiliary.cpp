//
//  auxiliary.cpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>

#include "auxiliary.hpp"

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
    edges.emplace_back(edge);
    node = graph.getEdge(edge).getSourceId();
  }
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
  // Trie node distance.
  trieSize = acAutomaton.getTrieSize();
  trieNodeDist.emplace_back(0);
  for (int i = 1; i < trieSize; ++i) {
    int father = acAutomaton.getFather(i);
    auto& fatherEdge = graph.getEdge(acAutomaton.getEdge(father));
    auto& edge = graph.getEdge(acAutomaton.getEdge(i));
    trieNodeDist.emplace_back(
      trieNodeDist.at(father) +
      edge.getDistance() +
      getSPDistance(fatherEdge.getTargetId(), edge.getSourceId())
    );
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

void Auxiliary::print() const {
  // TODO
}

Auxiliary::~Auxiliary() { }
