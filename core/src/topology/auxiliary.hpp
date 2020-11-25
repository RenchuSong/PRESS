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
  size_t nodeNumber;
  size_t edgeNumber;
  size_t trieSize;
  // Stores the road network distance between pair of nodes.
  std::vector<std::unordered_map<int, double> > nodePairSPDist;
  // Stores the distance and starting/ending node of each Trie node in ACActomaton.
  std::vector<double> trieNodeDist;
  std::vector<int> trieNodeStartNode;
  std::vector<int> trieNodeEndNode;
  // Stores the MBR of each edge in graph.
  std::vector<std::pair<Point2D, Point2D> > edgeMBR;
  // Stores the MBR of the shortest path of each pair of nodes in SP table.
  std::vector<std::unordered_map<int, std::pair<Point2D, Point2D> > > nodePairMBR;
  // Stores the MBR of each Trie node in ACAutomaton.
  std::vector<std::pair<Point2D, Point2D> > trieNodeMBR;

public:
  Auxiliary();
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
  // Get the trie node start node.
  int getTrieNodeStartNode(size_t index) const;
  // Get the trie node end node.
  int getTrieNodeEndNode(size_t index) const;
  // Get the shortest path distance between two nodes.
  std::pair<Point2D, Point2D> getNodePairMBR(
    const Graph& graph,
    size_t srcIndex,
    size_t tgtIndex
  ) const;
  // Get the trie node MBR.
  const std::pair<Point2D, Point2D>& getTrieNodeMBR(size_t index) const;
  // Get the edge MBR.
  const std::pair<Point2D, Point2D>& getEdgeMBR(size_t index) const;
  void build(const Graph& graph, const SPTable& spTable, const ACAutomaton& acAutomaton);
  void load(FileReader& auxReader);
  void clear();
  void print() const;
  ~Auxiliary();
};

#endif /* auxiliary_hpp */
