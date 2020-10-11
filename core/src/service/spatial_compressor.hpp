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

#include "../topology/ac_automaton.hpp"
#include "../topology/graph.hpp"
#include "../topology/huffman.hpp"
#include "../topology/sp_table.hpp"
#include "../io/binary.hpp"

class SpatialCompressor {
public:
  // Stage 1. Shortest path compression.
  void shortestPathCompression(
    const Graph& graph,
    const SPTable& spTable,
    const std::vector<int>& spatial,
    std::vector<int>& result
  ) const;
  // Stage 2. Frequent sub-trajectory compression.
  void frequentSubTrajectoryCompresson(
    const ACAutomaton& acAutomaton,
    const Huffman& huffman,
    const std::vector<int>& spatial,
    Binary& binary
  ) const;
  // Hybrid spatial compression.
  void hybridSpatialCompression(
    const Graph& graph,
    const SPTable& spTable,
    const ACAutomaton& acAutomaton,
    const Huffman& huffman,
    const std::vector<int>& spatial,
    Binary& binary
  ) const;
  // Stage 1. Decompress frequent sub-trajactory.
  void frequentSubTrajectoryDecompresson(
    const ACAutomaton& acAutomaton,
    const Huffman& huffman,
    const Binary& binary,
    std::vector<int>& spatial
  ) const;
  // Stage 2. Decompress shortest path.
  void shortestPathDecompression(
    const Graph& graph,
    const SPTable& spTable,
    const std::vector<int>& spatialComp,
    std::vector<int>& spatial
  ) const;
  // Hybrid spatial decompression.
  void hybridSpatialDecompression(
    const Graph& graph,
    const SPTable& spTable,
    const ACAutomaton& acAutomaton,
    const Huffman& huffman,
    const Binary& binary,
    std::vector<int>& spatial
  ) const;
  ~SpatialCompressor();
};


#endif /* spatial_compressor_hpp */
