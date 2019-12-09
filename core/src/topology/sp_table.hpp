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

// The shortest path table.
class SPTable {
private:
  size_t nodeNumber;
  // spTable[i][j] represents the last edge index of the shortest path
  // between node of index i and node of index j.
  // Attention: We are using index, not id here for performance reasons.
  std::vector<std::vector<size_t> > spTable;

public:
  // Read SP table from the file.
  SPTable(FileReader& spReader);
  // Calculate SP table of the graph.
  SPTable(Graph& graph);
  void store(FileWriter& spWriter);
  size_t getNodeNumber();
  const std::vector<std::vector<size_t> >& getSPTable();
  size_t prevEdgeIndex(size_t srcIndex, size_t tgtIndex);
  void print();
  ~SPTable();
};

#endif /* sp_table_hpp */
