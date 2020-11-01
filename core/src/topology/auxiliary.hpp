//
//  auxiliary.hpp
//  press-v3
//
//  Created by Renchu Song on 10/11/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef auxiliary_hpp
#define auxiliary_hpp

#include "../io/file_reader.hpp"
#include "../io/file_writer.hpp"
#include "./graph.hpp"
#include "./sp_table.hpp"

class SPTable;

class Auxiliary {
private:
  size_t nodeNumber;
  // Stores the road network distance between pair of nodes.
  std::vector<std::unordered_map<int, double> > nodePairSPDist;

public:
  // Construct auxiliary structure for queries on PRESSCompressedTrajectory.
  // TODO: add more structures.
  Auxiliary(const Graph& graph, const SPTable& spTable);
  Auxiliary(FileReader& auxReader);
  void store(FileWriter& auxWriter);
  size_t getNodeNumber() const;
  // Get the shortest path distance between two nodes.
  double getSPDistance(size_t srcIndex, size_t tgtIndex) const;
  void print() const;
  ~Auxiliary();
};

#endif /* auxiliary_hpp */
