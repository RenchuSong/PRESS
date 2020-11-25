//
//  grid_index.hpp
//  press-v3
//
//  Created by Renchu Song on 9/6/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef grid_index_hpp
#define grid_index_hpp

#include <vector>
#include <unordered_set>

#include "./geospatial_structures.hpp"
#include "./graph.hpp"

class GridIndex {
private:
  // Grid width/height in meters.
  double gridWidth;
  double gridHeight;
  // The bottom-left corner point.
  Point2D minPoint;
  // The top-right corner point.
  Point2D maxPoint;
  // The index from each grid cell to the set of edge ids.
  std::vector<std::unordered_set<int> > index;

public:
  GridIndex();
  // Build grid index given graph and index cell width/height.
  GridIndex(Graph& graph, double width, double height);
  // Load grid index from file.
  GridIndex(FileReader& gridIndexReader);
  // Search nearby edges.
  void search(const Point2D& position, double dist, std::unordered_set<int>& edges) const;
  void load(FileReader& gridIndexReader);
  void build(Graph& graph, double width, double height);
  void store(FileWriter& gridIndexWriter);
  void print() const;
  void clear();
  ~GridIndex();
};


#endif /* grid_index_hpp */
