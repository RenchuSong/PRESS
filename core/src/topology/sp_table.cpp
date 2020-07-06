//
//  sp_table.cpp
//  press-v3
//
//  Created by Renchu Song on 12/8/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>
#include <queue>

#include "sp_table.hpp"

// Read the SP table from a file.
SPTable::SPTable(FileReader& spReader) {
  nodeNumber = spReader.nextInt();
  spTable = new int*[nodeNumber];
  for (auto i = 0; i < nodeNumber; i++) {
    spTable[i] = new int[nodeNumber];
    for (auto j = 0; j < nodeNumber; j++) {
      spTable[i][j] = spReader.nextInt();
    }
  }
}

// Calculate the SP table of a given graph.
// Dijkstra algorithm.
SPTable::SPTable(Graph& graph) {
  nodeNumber = graph.getNodeNumber();

  // Init the SP table.
  spTable = new int*[nodeNumber];
  for (auto i = 0; i < nodeNumber; i++) {
    spTable[i] = new int[nodeNumber];
    for (auto j = 0; j < nodeNumber; j++) {
      spTable[i][j] = EDGE_NOT_EXIST;
    }
  }

  // Calculate all-pair shortest paths.
  for (auto i = 0; i < nodeNumber; i++) {
    float minDist[nodeNumber];
    for (auto j = 0; j < nodeNumber; j++) {
      minDist[j] = 1e20;
    }
    minDist[i] = 0;

    // Priority queue to hold the distance.
    std::priority_queue<
      std::pair<float, int>,
      std::vector<std::pair<float, int> >,
      std::greater<std::pair<float, int> >
    > queue;
    queue.push(std::make_pair(0, i));

    // Loop nodeNumber times, always pop the node with smallest distance.
    while (!queue.empty()) {
      auto srcId = queue.top().second;
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
          spTable[i][tgtId] = edgeId;
          queue.push(std::make_pair(minDist[tgtId], tgtId));
        }
      }
    }
  }
}

// Store the SP table into the file.
void SPTable::store(FileWriter& spWriter) {
  spWriter.writeInt((int)nodeNumber);
  spWriter.writeEol();
  for (auto i = 0; i < nodeNumber; i++) {
    for (auto j = 0; j < nodeNumber; j++) {
      spWriter.writeInt((int)spTable[i][j]);
      spWriter.writeSeparator();
    }
    spWriter.writeEol();
  }
}

// Get the node number.
size_t SPTable::getNodeNumber() {
  return nodeNumber;
}

// Get the SP table.
int** SPTable::getSPTable() {
  return spTable;
}

// Get the last edge index along the shortest path between node srcIndex and node tgtIndex.
int SPTable::prevEdgeIndex(size_t srcIndex, size_t tgtIndex) {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  return spTable[srcIndex][tgtIndex];
}

// Append the shortest path sequence (edge1, edge2] to container.
void SPTable::complement(Graph& graph, int edge1, int edge2, std::vector<int>& container) {
  std::vector<int> tmpPath;
  auto node1 = graph.getEdge(edge1).getTargetId();
  while (edge2 != EDGE_NOT_EXIST) {
    tmpPath.emplace_back(edge2);
    auto node2 = graph.getEdge(edge2).getSourceId();
    if (node1 == node2) {
      break;
    }
    edge2 = spTable[node1][node2];
  }
  std::reverse(tmpPath.begin(), tmpPath.end());
  container.insert(container.end(), tmpPath.begin(), tmpPath.end());
}

// Print the SP table for debug.
void SPTable::print() {
  std::cout << nodeNumber << std::endl;
  for (auto i = 0; i < nodeNumber; i++) {
    for (auto j = 0; j < nodeNumber; j++) {
      std::cout << spTable[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

SPTable::~SPTable() {
  for (auto i = 0; i < nodeNumber; i++) {
    delete[] spTable[i];
  }
  delete[] spTable;
}
