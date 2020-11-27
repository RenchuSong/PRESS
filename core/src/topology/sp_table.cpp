//
//  optimized_sp_table.cpp
//  press-v3
//
//  Created by Renchu Song on 9/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//
#include <cassert>
#include <algorithm>
#include <queue>

#include "sp_table.hpp"

SPTable::SPTable() { }

// Read the optimized SP table from a file.
SPTable::SPTable(FileReader& spReader) {
  load(spReader);
}

// Calculate the optimized SP table of a given graph.
// Dijkstra algorithm.
SPTable::SPTable(Graph& graph, double maxDist) {
  build(graph, maxDist);
}

void SPTable::load(FileReader& spReader) {
  clear();
  nodeNumber = spReader.nextInt();
  for (auto i = 0; i < nodeNumber; i++) {
    std::unordered_map<int, int> nodePrevEdge;
    int prevNum = spReader.nextInt();
    for (int j = 0; j < prevNum; ++j) {
      int eid = spReader.nextInt();
      int prevEid = spReader.nextInt();
      nodePrevEdge[eid] = prevEid;
    }
    prevEdge.emplace_back(nodePrevEdge);
  }
}

// Calculate the optimized SP table of a given graph.
// Dijkstra algorithm.
void SPTable::build(Graph& graph, double maxDist) {
  clear();
  nodeNumber = graph.getNodeNumber();

  // Calculate all-pair shortest paths.
  for (auto i = 0; i < nodeNumber; i++) {
    std::unordered_map<int, int> nodePrevEdge;

    double minDist[nodeNumber];
    int prev[nodeNumber];
    for (auto j = 0; j < nodeNumber; j++) {
      minDist[j] = 1e20;
      prev[j] = EDGE_NOT_EXIST;
    }
    minDist[i] = 0;

    // Priority queue to hold the distance.
    std::priority_queue<
      std::pair<double, int>,
      std::vector<std::pair<double, int> >,
      std::greater<std::pair<double, int> >
    > queue;
    queue.push(std::make_pair(0, i));

    // Loop nodeNumber times, always pop the node with smallest distance.
    while (!queue.empty()) {
      auto& candidate = queue.top();
      if (candidate.first > maxDist) {
        break;
      }
      auto srcId = candidate.second;
      nodePrevEdge[srcId] = prev[srcId];
      queue.pop();
      auto& node = graph.getNode(srcId);
      auto& edgeList = node.getEdgeList();
      for (auto edgeId: edgeList) {
        auto& edge = graph.getEdge(edgeId);
        auto tgtId = edge.getTargetId();
        // Relaxation function.
        if (minDist[tgtId] > minDist[srcId] + edge.getDistance()) {
          // Update min distance.
          minDist[tgtId] = minDist[srcId] + edge.getDistance();
          // Update SP table, and update queue.
          prev[tgtId] = edgeId;
          queue.push(std::make_pair(minDist[tgtId], tgtId));
        }
      }
    }
    prevEdge.emplace_back(nodePrevEdge);
  }
}

// Store the optimized SP table into the file.
void SPTable::store(FileWriter& spWriter) {
  spWriter.writeInt((int)nodeNumber);
  spWriter.writeEol();
  for (auto i = 0; i < nodeNumber; i++) {
    spWriter.writeInt((int)prevEdge.at(i).size());
    for (auto& eidPrevPair: prevEdge.at(i)) {
      spWriter.writeSeparator();
      spWriter.writeInt(eidPrevPair.first);
      spWriter.writeSeparator();
      spWriter.writeInt(eidPrevPair.second);
    }
    spWriter.writeEol();
  }
}

// Get the node number.
size_t SPTable::getNodeNumber() const {
  return nodeNumber;
}

// Get the last edge index along the shortest path between node srcIndex and node tgtIndex.
int SPTable::prevEdgeIndex(size_t srcIndex, size_t tgtIndex) const {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  auto& prev = prevEdge.at(srcIndex);
  if (prev.find((int)tgtIndex) != prev.end()) {
    return prev.at((int)tgtIndex);
  } else {
    return EDGE_NOT_EXIST;
  }
}

// Append the shortest path sequence (edge1, edge2] to container.
void SPTable::complement(
  const Graph& graph,
  int edge1,
  int edge2,
  std::vector<int>& container
) const {
  auto node1 = graph.getEdge(edge1).getTargetId();
  auto node2 = graph.getEdge(edge2).getSourceId();
  complementNode(graph, node1, node2, container);
  container.emplace_back(edge2);
}

// Append the shortest path sequence (node1, node2) to container.
void SPTable::complementNode(
  const Graph& graph,
  int node1,
  int node2,
  std::vector<int>& container
) const {
  std::vector<int> tmpPath;
  while (node1 != node2) {
    int edge2 = prevEdgeIndex(node1, node2);
    if (edge2 == EDGE_NOT_EXIST) {
      break;
    }
    tmpPath.emplace_back(edge2);
    node2 = graph.getEdge(edge2).getSourceId();
  }
  std::reverse(tmpPath.begin(), tmpPath.end());
  container.insert(container.end(), tmpPath.begin(), tmpPath.end());
}

// Print the optimized SP table for debug.
void SPTable::print() const {
  std::cout << nodeNumber << std::endl;
  for (auto i = 0; i < nodeNumber; i++) {
    std::cout << i << ": ";
    for (auto& eidPrevPair: prevEdge.at(i)) {
      std::cout << "(" << eidPrevPair.first << ", " << eidPrevPair.second << ") ";
    }
    std::cout << std::endl;
  }
}

void SPTable::clear() {
  nodeNumber = 0;
  prevEdge.clear();
}

SPTable::~SPTable() { }
