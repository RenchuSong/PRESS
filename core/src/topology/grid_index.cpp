//
//  grid_index.cpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "grid_index.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

#include "../util/helper.hpp"
#include "../third_party/log.h"

// Return cell (x, y) in grid (col, row)
int getCellIndex(int col, int x, int y) {
  return y * col + x;
}

GridIndex::GridIndex() { }

// Build grid index given graph and index cell width/height.
GridIndex::GridIndex(Graph& graph, double width, double height) {
  build(graph, width, height);
}

// Read grid index from file.
GridIndex::GridIndex(FileReader& gridIndexReader) {
  load(gridIndexReader);
}

// Search nearby edges.
void GridIndex::search(const Point2D& position, double dist, std::unordered_set<int>& edges) const {
  edges.clear();

  // Find the cells.
  int col = (int)round((maxPoint.x - minPoint.x) / gridWidth);
  int row = (int)round((maxPoint.y - minPoint.y) / gridHeight);
  int col1 = std::max((int)floor((position.x - dist - minPoint.x) / gridWidth), 0);
  int row1 = std::max((int)floor((position.y - dist - minPoint.y) / gridHeight), 0);
  int col2 = std::min((int)floor((position.x + dist - minPoint.x) / gridWidth), col - 1);
  int row2 = std::min((int)floor((position.y + dist - minPoint.y) / gridHeight), row - 1);

  // Collect the results.
  for (int r = row1; r <= row2; ++r) {
    for (int c = col1; c <= col2; ++c) {
      // If the cell's distance to the point is larger than dist, skip.
      Point2D p1(c * gridWidth + minPoint.x, r * gridHeight + minPoint.y);
      Point2D p2((c + 1) * gridWidth + minPoint.x, r * gridHeight + minPoint.y);
      Point2D p3(c * gridWidth + minPoint.x, (r + 1) * gridHeight + minPoint.y);
      Point2D p4((c + 1) * gridWidth + minPoint.x, (r + 1) * gridHeight + minPoint.y);
      if (position.x < p1.x || position.y < p1.y || position.x > p4.x || position.y > p4.y) {
        if (
          distPoint2Interval(position, p1, p2) > dist &&
          distPoint2Interval(position, p1, p3) > dist &&
          distPoint2Interval(position, p2, p4) > dist &&
          distPoint2Interval(position, p3, p4) > dist
        ) {
          continue;
        }
      }

      int cellIndex = getCellIndex(col, c, r);
      edges.insert(index.at(cellIndex).begin(), index.at(cellIndex).end());
    }
  }
}

// Read grid index from file.
void GridIndex::load(FileReader& gridIndexReader) {
  clear();
  gridWidth = (gridIndexReader.nextDouble());
  gridHeight = (gridIndexReader.nextDouble());
  minPoint.setPosition(gridIndexReader.nextDouble(), gridIndexReader.nextDouble());
  maxPoint.setPosition(gridIndexReader.nextDouble(), gridIndexReader.nextDouble());
  int gridSize = gridIndexReader.nextInt();
  for (auto i = 0; i < gridSize; ++i) {
    int edgeNumber = gridIndexReader.nextInt();
    std::unordered_set<int> edges;
    for (auto j = 0; j < edgeNumber; ++j) {
      edges.insert(gridIndexReader.nextInt());
    }
    index.emplace_back(edges);
  }
}

void GridIndex::build(Graph& graph, double width, double height) {
  if (width <= 0 || height <= 0) {
    FILE_LOG(TLogLevel::lerror)
      << "Cannot build grid index with non-positive cell width or height: "
      << width << ", " << height;
    throw "Cannot build grid index with non-positive cell width or height.";
  }
  clear();
  gridWidth = width;
  gridHeight = height;
  // Get the boundary of the graph.
  minPoint = graph.getNode(0).getPosition();
  maxPoint = graph.getNode(0).getPosition();
  for (auto& edge: graph.getEdgeList()) {
    for (auto& point: edge.getShape()) {
      minPoint.setPosition(std::min(minPoint.x, point.x), std::min(minPoint.y, point.y));
      maxPoint.setPosition(std::max(maxPoint.x, point.x), std::max(maxPoint.y, point.y));
    }
  }
  // Normalize the boundary based on width/height.
  minPoint.setPosition(floor(minPoint.x / width) * width, floor(minPoint.y / height) * height);
  maxPoint.setPosition(ceil(maxPoint.x / width) * width, ceil(maxPoint.y / height) * height);
  
  int col = round((maxPoint.x - minPoint.x) / width);
  int row = round((maxPoint.y - minPoint.y) / height);
  // Init the index.
  for (int i = 0; i < col * row; ++i) {
    index.emplace_back(std::unordered_set<int> {});
  }
  
  // Insert each edge to the cells.
  for (int edgeId = 0; edgeId < graph.getEdgeNumber(); ++edgeId) {
    auto& shape = graph.getEdge(edgeId).getShape();
    std::unordered_set<int> insertedCells;
    // Insert the points to corresponding cells.
    for (auto& point: shape) {
      int x = (int)floor((point.x - minPoint.x) / width);
      int y = (int)floor((point.y - minPoint.y) / height);
      int cellIndex = getCellIndex(col, x, y);
      index.at(cellIndex).insert(edgeId);
      insertedCells.insert(cellIndex);
    }
    // Check if each segment of shape intersects with cells.
    for (auto i = 1; i < shape.size(); ++i) {
      int col1 = (int)floor((shape.at(i - 1).x - minPoint.x) / width);
      int row1 = (int)floor((shape.at(i - 1).y - minPoint.y) / height);
      int col2 = (int)floor((shape.at(i).x - minPoint.x) / width);
      int row2 = (int)floor((shape.at(i).y - minPoint.y) / height);
      if (col1 > col2) {
        std::swap(col1, col2);
      }
      if (row1 > row2) {
        std::swap(row1, row2);
      }
      for (auto r = row1; r <= row2; ++r) {
        for (auto c = col1; c <= col2; ++c) {
          // Only check cells not containing the edge.
          int cellIndex = getCellIndex(col, c, r);
          if (insertedCells.find(cellIndex) != insertedCells.end()) {
            continue;
          }
          Point2D minBound(c * width + minPoint.x, r * height + minPoint.y);
          Point2D maxBound((c + 1) * width + minPoint.x, (r + 1) * height + minPoint.y);
          if (intervalThroughMBR(shape.at(i - 1), shape.at(i), minBound, maxBound)) {
            index.at(cellIndex).insert(edgeId);
            insertedCells.insert(cellIndex);
          }
        }
      }
    }
  }
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
      gridIndexWriter.writeInt(edge);
    }
    gridIndexWriter.writeEol();
  }
}

void GridIndex::print() const {
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

void GridIndex::clear() {
  index.clear();
}

GridIndex::~GridIndex() { }
