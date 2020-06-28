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
    Graph& graph,
    SPTable& spTable,
    const std::vector<int>& spatial,
    std::vector<int>& result
  );
  // Stage 2. Frequent sub-trajectory compression.
  void frequentSubTrajectoryCompresson(
    ACAutomaton& acAutomaton,
    Huffman& huffman,
    const std::vector<int>& spatial,
    std::vector<bool>& binary
  );
  // Hybrid spatial compression.
  void hybridSpatialCompression(
    Graph& graph,
    SPTable& spTable,
    ACAutomaton& acAutomaton,
    Huffman& huffman,
    const std::vector<int>& spatial,
    std::vector<bool>& binary
  );

  ~SpatialCompressor();
};


#endif /* spatial_compressor_hpp */
