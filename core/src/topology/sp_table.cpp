//
//  sp_table.cpp
//  press-v3
//
//  Created by Renchu Song on 12/8/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>

#include "sp_table.hpp"

// Read the SP table from a file.
SPTable::SPTable(FileReader& spReader) {
  nodeNumber = spReader.nextInt();
  for (auto i = 0; i < nodeNumber; i++) {
    spTable.emplace_back(std::vector<size_t>());
    for (auto j = 0; j < nodeNumber; j++) {
      spTable[i].emplace_back(spReader.nextInt());
    }
  }
}

// Calculate the SP table of a given graph.
SPTable::SPTable(Graph& graph) {
  // TODO
  nodeNumber = graph.getNodeNumber();
  for (auto i = 0; i < nodeNumber; i++) {
    spTable.emplace_back(std::vector<size_t>());
    for (auto j = 0; j < nodeNumber; j++) {
      spTable[i].emplace_back(0);
    }
  }
}

// Store the SP table into the file.
void SPTable::store(FileWriter& spWriter) {
  spWriter.writeInt((int)nodeNumber);
  if (spWriter.isBinary()) {
    for (auto i = 0; i < nodeNumber; i++) {
      for (auto j = 0; j < nodeNumber; j++) {
        spWriter.writeInt((int)spTable[i][j]);
      }
    }
  } else {
    for (auto i = 0; i < nodeNumber; i++) {
      for (auto j = 0; j < nodeNumber; j++) {
        spWriter.writeChar(' ');
        spWriter.writeInt((int)spTable[i][j]);
      }
    }
  }
}

// Get the node number.
size_t SPTable::getNodeNumber() {
  return nodeNumber;
}

// Get the SP table.
const std::vector<std::vector<size_t> >& SPTable::getSPTable() {
  return spTable;
}

// Get the last edge index along the shortest path between node srcIndex and node tgtIndex.
size_t SPTable::prevEdgeIndex(size_t srcIndex, size_t tgtIndex) {
  assert(srcIndex < nodeNumber && tgtIndex < nodeNumber);
  return spTable[srcIndex][tgtIndex];
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

SPTable::~SPTable() { }
