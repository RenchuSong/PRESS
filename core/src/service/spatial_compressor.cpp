//
//  spatial_compressor.cpp
//  press-v3
//
//  Created by Renchu Song on 5/31/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>

#include "spatial_compressor.hpp"

// Construct spatial compressor.
SpatialCompressor::SpatialCompressor(
  Graph* graph,
  SPTable* spTable
): graph(graph), spTable(spTable) { }

// Stage 1. Shortest path compression.
void SpatialCompressor::shortestPathCompression(
  const std::vector<int> &spatial,
  std::vector<int> &result
) {
  result.clear();

  // Empty path.
  size_t len = spatial.size();
  if (len == 0) {
    return;
  }

  // Always add starting edge to result.
  result.emplace_back(spatial.at(0));

  // Adding all the important edges to result.
  int nodeId = graph->getEdge(spatial.at(0)).getTargetId();
  for (size_t i = 1; i < len - 1; i++) {
    const int edgeId = spatial.at(i);
    const int tgtId = graph->getEdge(edgeId).getTargetId();
    if (spTable->prevEdgeIndex(nodeId, tgtId) != edgeId) {
      result.emplace_back(edgeId);
      nodeId = tgtId;
    }
  }

  // Always add ending edge to result.
  if (len > 1) {
    result.emplace_back(spatial.at(len - 1));
  }
}

// Stage 2. Frequent sub-trajectory compression.
void SpatialCompressor::frequentSubTrajectoryCompresson(
  const std::vector<int>& spatial,
  std::vector<bool>& binary
) {
  // TODO: to be implemented.
}

// Hybrid spatial compression.
void SpatialCompressor::hybridSpatialCompression(
  const std::vector<int>& spatial,
  std::vector<bool>& binary
) {
  std::vector<int> tmpSpatial;
  shortestPathCompression(spatial, tmpSpatial);
  frequentSubTrajectoryCompresson(tmpSpatial, binary);
}

SpatialCompressor::~SpatialCompressor() { }
