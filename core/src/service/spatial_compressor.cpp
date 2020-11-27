//
//  spatial_compressor.cpp
//  press-v3
//
//  Created by Renchu Song on 5/31/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <cassert>
#include <algorithm>

#include "spatial_compressor.hpp"

// Stage 1. Shortest path compression.
void SpatialCompressor::shortestPathCompression(
  const Graph& graph,
  const SPTable& spTable,
  const std::vector<int>& spatial,
  std::vector<int>& result
) const {
  result.clear();

  // Empty path.
  size_t len = spatial.size();
  if (len == 0) {
    return;
  }

  // Always add starting edge to result.
  result.emplace_back(spatial.at(0));

  // Adding all the important edges to result.
  int nodeId = graph.getEdge(spatial.at(0)).getTargetId();
  for (size_t i = 1; i < len - 1; i++) {
    const int edgeId = spatial.at(i);
    const int tgtId = graph.getEdge(edgeId).getTargetId();
    if (
      (
        i < len - 2 &&
        graph.getEdge(spatial.at(i)).getTargetId() != graph.getEdge(spatial.at(i + 1)).getSourceId()
      ) ||
      spTable.prevEdgeIndex(nodeId, tgtId) != edgeId
    ) {
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
  const ACAutomaton& acAutomaton,
  const Huffman& huffman,
  const std::vector<int>& spatial,
  Binary& binary
) const {
  // Get matched trie nodes sequence.
  std::vector<int> trieNodeSequence;
  int id = 0;
  for (auto edgeId: spatial) {
    id = acAutomaton.getMatch(id, edgeId);
    trieNodeSequence.emplace_back(id);
  }

  // Decomposite the spatial sequence.
  std::vector<int> decomposited;
  id = 0;
  for (int i = (int)trieNodeSequence.size() - 1; i >= 0; i--) {
    if (id == 0) {
      id = trieNodeSequence.at(i);
      decomposited.emplace_back(id);
    }
    id = acAutomaton.getFather(id);
  }
  std::reverse(decomposited.begin(), decomposited.end());

  // Encode.
  std::vector<bool> binaryCode;
  huffman.encode(decomposited, binaryCode);
  binary.setBinary(binaryCode);
}

// Hybrid spatial compression.
void SpatialCompressor::hybridSpatialCompression(
  const Graph& graph,
  const SPTable& spTable,
  const ACAutomaton& acAutomaton,
  const Huffman& huffman,
  const std::vector<int>& spatial,
  Binary& binary
) const {
  std::vector<int> tmpSpatial;
  shortestPathCompression(graph, spTable, spatial, tmpSpatial);
  frequentSubTrajectoryCompresson(acAutomaton, huffman, tmpSpatial, binary);
}

// Stage 1. Decompress frequent sub-trajactory.
void SpatialCompressor::frequentSubTrajectoryDecompresson(
  const ACAutomaton& acAutomaton,
  const Huffman& huffman,
  const Binary& binary,
 std::vector<int>& spatial
) const {
  // Decode to decomposited spatial sequence.
  std::vector<bool> binaryCode;
  std::vector<int> decomposited;
  binary.getBitArray(binaryCode);
  huffman.decode(binaryCode, decomposited);

  // Recover spatial sequence.
  spatial.clear();
  for (int i = (int)decomposited.size() - 1; i >= 0; --i) {
    int acId = decomposited.at(i);
    while (acId != ROOT_NODE) {
      spatial.emplace_back(acAutomaton.getEdge(acId));
      acId = acAutomaton.getFather(acId);
    }
  }
  std::reverse(spatial.begin(), spatial.end());
}

// Stage 2. Decompress shortest path.
void SpatialCompressor::shortestPathDecompression(
  const Graph& graph,
  const SPTable& spTable,
  const std::vector<int>& spatialComp,
  std::vector<int>& spatial
) const {
  spatial.clear();
  if (spatialComp.size() == 0) {
    return;
  }
  spatial.emplace_back(spatialComp.at(0));
  auto spatialCompSize = spatialComp.size();
  for (int i = 1; i < spatialCompSize; ++i) {
    spTable.complement(graph, spatialComp.at(i - 1), spatialComp.at(i), spatial);
  }
}

// Hybrid spatial decompression.
void SpatialCompressor::hybridSpatialDecompression(
  const Graph& graph,
  const SPTable& spTable,
  const ACAutomaton& acAutomaton,
  const Huffman& huffman,
  const Binary& binary,
  std::vector<int>& spatial
) const {
  std::vector<int> tmpSpatial;
  frequentSubTrajectoryDecompresson(acAutomaton, huffman, binary, tmpSpatial);
  shortestPathDecompression(graph, spTable, tmpSpatial, spatial);
}

SpatialCompressor::~SpatialCompressor() { }
