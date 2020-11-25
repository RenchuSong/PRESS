//
//  optimized_sp_table.hpp
//  press-v3
//
//  The original paper stored all-edge-pair SP table. In this implementation, we optimized to only
//  store the intersection-pair SP (instead of edge-edge pair), and we only store the distance from
//  each intersection within maxDist (to make it tunable to avoid OOM for huge roadnet).
//
//  Created by Renchu Song on 9/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef optimized_sp_table_hpp
#define optimized_sp_table_hpp


#include <vector>
#include <unordered_map>

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "./auxiliary.hpp"
#include "./graph.hpp"

const int EDGE_NOT_EXIST = -1;

// The optimized shortest path table (via limiting edges).
class SPTable {
private:
  size_t nodeNumber;
  std::vector<std::unordered_map<int, int> > prevEdge;
  friend class Auxiliary;

public:
  SPTable();
  // Read optimized SP table from the file.
  SPTable(FileReader& spReader);
  // Calculate optimized SP table of the graph. Only store edges within the maxDist.
  SPTable(Graph& graph, double maxDist);
  void build(Graph& graph, double maxDist);
  void load(FileReader& spReader);
  void store(FileWriter& spWriter);
  size_t getNodeNumber() const;
  int prevEdgeIndex(size_t srcIndex, size_t tgtIndex) const;
  // Append the shortest path sequence (edge1, edge2] to container.
  void complement(const Graph& graph, int edge1, int edge2, std::vector<int>& container) const;
  // Append the shortest path sequence (node1, node2) to container.
  void complementNode(const Graph& graph, int node1, int node2, std::vector<int>& container) const;
  void print() const;
  void clear();
  ~SPTable();
};

#endif /* optimized_sp_table_hpp */
