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
#include "./ac_automaton.hpp"
#include "./graph.hpp"
#include "./sp_table.hpp"

class SPTable;

class Auxiliary {
private:
  // Stores the road network distance between pair of nodes.
  size_t nodeNumber;
  std::vector<std::unordered_map<int, double> > nodePairSPDist;
  // Stores the distance of each Trie node in ACActomaton.
  size_t trieSize;
  std::vector<double> trieNodeDist;

public:
  // Construct auxiliary structure for queries on PRESSCompressedTrajectory.
  // TODO: add more structures.
  Auxiliary(const Graph& graph, const SPTable& spTable, const ACAutomaton& acAutomaton);
  Auxiliary(FileReader& auxReader);
  void store(FileWriter& auxWriter);
  size_t getNodeNumber() const;
  // Get the shortest path distance between two nodes.
  double getSPDistance(size_t srcIndex, size_t tgtIndex) const;
  // Get the trie node distance.
  double getTrieNodeDistance(size_t index) const;
  void print() const;
  ~Auxiliary();
};

#endif /* auxiliary_hpp */
