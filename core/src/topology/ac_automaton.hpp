//
//  ac_automaton.hpp
//  press-v3
//
//  Created by Renchu Song on 6/20/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef ac_automaton_hpp
#define ac_automaton_hpp

#include <deque>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "./graph.hpp"

class ACAutomaton {
private:
  // Each Trie node has a map from succeeding edge id in graph to the corresponding trie node index.
  std::vector<std::unordered_map<int, int> > trie;
  // Corresponding edge id of each trie node.
  std::vector<int> edgeId;
  // Father node of each trie node.
  std::vector<int> father;
  // Next position for a failed match.
  std::vector<int> next;
  // Frequency of each trie node.
  std::vector<int> frequency;
  // Trie size.
  size_t trieSize;
  // Add one sequence of given frequency.
  void insertSequence(std::deque<int>& sequence, int occur);

public:
  // Read AC automaton from file.
  ACAutomaton(FileReader& acReader);
  // Construct AC automaton from shortest path compressed spatial components.
  ACAutomaton(Graph& graph, std::vector<std::vector<int> >& spCompressedSpatialComps, int theta);
  // Store the AC automaton to file.
  void store(FileWriter& acWriter);
  // Print for debug.
  void print();
  // Given previous Trie matching position, get the next matched trie node.
  int getMatch(int prevPos, int edgeId);
  // Get corresponding edge id of a trie node.
  int getEdge(int index);
  // Get corresponding frequency of a trie node.
  int getFrequency(int index);
  // Get corresponding fathre trie node index of a trie node.
  int getFather(int index);
  // Get frequency of all trie nodes.
  std::vector<int>& getAllFrequency();
  ~ACAutomaton();
};

#endif /* ac_automaton_hpp */
