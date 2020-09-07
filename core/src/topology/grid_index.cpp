//
//  grid_index.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "grid_index.hpp"

#include <iostream>

// Build grid index given graph and index cell width/height.
GridIndex::GridIndex(Graph& graph, int width, int height): gridWidth(width), gridHeight(height) {
  // TODO
}

// Read grid index from file.
GridIndex::GridIndex(FileReader& gridIndexReader):
  gridWidth(gridIndexReader.nextDouble()),
  gridHeight(gridIndexReader.nextDouble()),
  minPoint(gridIndexReader.nextDouble(), gridIndexReader.nextDouble()),
  maxPoint(gridIndexReader.nextDouble(), gridIndexReader.nextDouble())
{
  int gridSize = gridIndexReader.nextInt();
  for (auto i = 0; i < gridSize; ++i) {
    int edgeNumber = gridIndexReader.nextInt();
    std::vector<int> edges;
    for (auto j = 0; j < edgeNumber; ++j) {
      edges.emplace_back(gridIndexReader.nextInt());
    }
    index.emplace_back(edges);
  }
}

// Search nearby edges.
void GridIndex::search(Point2D& position, double dist, std::vector<int>& edges) {
  edges.clear();
  // TODO
}

// Write grid index to file.
void GridIndex::store(FileWriter& gridIndexWriter) {
  gridIndexWriter.writeDouble(gridWidth);
  gridIndexWriter.writeSeparator();
  gridIndexWriter.writeDouble(gridHeight);
  gridIndexWriter.writeSeparator();
  gridIndexWriter.writeDouble(minPoint.x);
  gridIndexWriter.writeSeparator();
  gridIndexWriter.writeDouble(minPoint.y);
  gridIndexWriter.writeSeparator();
  gridIndexWriter.writeDouble(maxPoint.x);
  gridIndexWriter.writeSeparator();
  gridIndexWriter.writeDouble(maxPoint.y);
  gridIndexWriter.writeEol();
  gridIndexWriter.writeInt((int)index.size());
  gridIndexWriter.writeEol();
  for (auto& edges: index) {
    gridIndexWriter.writeInt((int)edges.size());
    for (auto edge: edges) {
      gridIndexWriter.writeSeparator();
      gridIndexWriter.writeDouble(edge);
    }
    gridIndexWriter.writeEol();
  }
}

void GridIndex::print() {
  std::cout << gridWidth << " " << gridHeight << std::endl;
  minPoint.print();
  maxPoint.print();
  std::cout << std::endl;
  for (auto& edges: index) {
    for (auto edge: edges) {
      std::cout << edge << ",";
    }
    std::cout << std::endl;
  }
}

GridIndex::~GridIndex() { }
