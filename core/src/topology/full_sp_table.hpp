//
//  sp_table.hpp
//  press-v3
//
//  Created by Renchu Song on 12/8/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef sp_table_hpp
#define sp_table_hpp

#include <vector>

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "./graph.hpp"

const int EDGE_NOT_EXIST = -1;

// The shortest path table.
class FullSPTable {
private:
  size_t nodeNumber;
  // spTable[i][j] represents the last edge index of the shortest path
  // between node of index i and node of index j.
  // Attention: We are using index, not id here for performance reasons.
  int** spTable;
  
public:
  // Read SP table from the file.
  FullSPTable(FileReader& spReader);
  // Calculate SP table of the graph.
  FullSPTable(Graph& graph);
  void store(FileWriter& spWriter);
  size_t getNodeNumber() const;
  int** getSPTable() const;
  int prevEdgeIndex(size_t srcIndex, size_t tgtIndex) const;
  // Append the shortest path sequence (edge1, edge2] to container.
  void complement(const Graph& graph, int edge1, int edge2, std::vector<int>& container) const;
  void print() const;
  ~FullSPTable();
};

#endif /* sp_table_hpp */
