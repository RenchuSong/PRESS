//
//  spatial_compressor.hpp
//  press-v3
//
//  Created by Renchu Song on 5/31/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef spatial_compressor_hpp
#define spatial_compressor_hpp

#include <vector>

#include "../topology/graph.hpp"
#include "../topology/sp_table.hpp"

class SpatialCompressor {
private:
  Graph* graph;
  SPTable* spTable;

public:
  // Construct SpatialCompressor service.
  SpatialCompressor(Graph* graph, SPTable* spTable);
  // Stage 1. Shortest path compression.
  void shortestPathCompression(const std::vector<int>& spatial, std::vector<int>& result);
  // Stage 2. Frequent sub-trajectory compression.
  void frequentSubTrajectoryCompresson(const std::vector<int>& spatial, std::vector<bool>& binary);
  // Hybrid spatial compression.
  void hybridSpatialCompression(const std::vector<int>& spatial, std::vector<bool>& binary);

  ~SpatialCompressor();
};


#endif /* spatial_compressor_h */
