//
//  ac_automaton.cpp
//  press-v3
//
//  Created by Renchu Song on 6/20/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <queue>
#include <utility>

#include "ac_automaton.hpp"

ACAutomaton::ACAutomaton() { }

// Read AC automaton from file.
ACAutomaton::ACAutomaton(FileReader& acReader) {
  load(acReader);
}

// Construct AC automaton from shortest path compressed spatial components.
ACAutomaton::ACAutomaton(
 const Graph& graph,
 const std::vector<std::vector<int> >& spCompressedSpatialComps,
 int theta
) {
  build(graph, spCompressedSpatialComps, theta);
}

// Insert one sequence into the trie.
void ACAutomaton::insertSequence(std::deque<int>& sequence, int occur) {
  int total = (int)trie.size() - 1;
  int cur = 0;
  for (int c : sequence) {
    if (trie[cur].find(c) != trie[cur].end()) {
      cur = trie[cur][c];
    } else {
      std::unordered_map<int, int> newNode;
      trie.emplace_back(newNode);
      edgeId.emplace_back(c);
      father.emplace_back(cur);
      frequency.emplace_back(0);
      total++;
      trie[cur][c] = total;
      cur = total;
    }
    frequency[cur] += occur;
  }
}

// Store the AC automaton to file.
void ACAutomaton::store(FileWriter& acWriter) {
  acWriter.writeInt((int)trieSize);
  acWriter.writeEol();
  for (int i = 0; i < trieSize; i++) {
    acWriter.writeInt(edgeId.at(i));
    acWriter.writeSeparator();
    acWriter.writeInt(father.at(i));
    acWriter.writeSeparator();
    acWriter.writeInt(next.at(i));
    acWriter.writeSeparator();
    acWriter.writeInt(frequency.at(i));
    acWriter.writeSeparator();
    auto& children = trie.at(i);
    acWriter.writeInt((int)children.size());
    for (auto it = children.begin(); it != children.end(); it++) {
      acWriter.writeSeparator();
      acWriter.writeInt(it->first);
      acWriter.writeSeparator();
      acWriter.writeInt(it->second);
    }
    acWriter.writeEol();
  }
}

void ACAutomaton::build(
  const Graph& graph,
  const std::vector<std::vector<int> >& spCompressedSpatialComps,
  int theta
) {
  clear();

  // Prepare trie root.
  std::unordered_map<int, int> root;
  trie.emplace_back(root);
  edgeId.emplace_back(-1);
  father.emplace_back(0);
  frequency.emplace_back(0);

  // Temp sequence container.
  std::deque<int> sequence;

  // Insert each edge into the trie.
  for (int i = 0; i < graph.getEdgeNumber(); i++) {
    sequence.push_back(i);
    // Since this is virtual the occurence frequency is 0.
    insertSequence(sequence, 0);
    sequence.pop_back();
  }

  // Insert all sub-sequence for each spatial component of length up to theta.
  for (auto& spatialComp: spCompressedSpatialComps) {
    auto spatialSize = spatialComp.size();
    int ptr = 0;
    for (; ptr < theta && ptr < spatialSize; ptr++) {
      sequence.push_back(spatialComp.at(ptr));
    }
    for (int i = 0; i < spatialSize; i++) {
      // The occurence frequency is 1.
      insertSequence(sequence, 1);
      sequence.pop_front();
      if (ptr < spatialSize) {
        sequence.push_back(spatialComp.at(ptr));
        ptr++;
      }
    }
  }

  // Update trie size.
  trieSize = trie.size();

  // Maintain fail pointer, so each time when attempting further match fails,
  // jump to the position where the longest suffix of current path is a prefix in the Trie.
  for (int i = 0; i < trieSize; i++) {
    next.emplace_back(0);
  }

  // Need follow BFS order to create next pointers so that upper level nodes
  // should have their correct failing jumping positions when processing a lower node in Trie.
  std::queue<std::pair<int, std::unordered_map<int, int> > > nodes;
  for (auto kv: trie[0]) {
    auto o = make_pair(kv.second, trie[kv.second]);
    nodes.push(o);
  }
  while (!nodes.empty()) {
    auto p = nodes.front();
    nodes.pop();
    auto i = p.first;
    auto node = p.second;
    for (auto kv : node) {
      auto pp = make_pair(kv.second, trie[kv.second]);
      nodes.push(pp);
    }
    for (auto kv : node) {
      char child = kv.first;
      int pos = kv.second;
      int f = next[i];
      // Very similar with KMP algorithm to find the failing position, but here the jumping can
      // be anywhere on the Trie.
      while (f != 0 && trie[f].find(child) == trie[f].end()) {
        f = next[f];
      }
      if (trie[f].find(child) != trie[f].end()) {
        f = trie[f][child];
      }
      next[pos] = f;
    }
  }
}

void ACAutomaton::load(FileReader& acReader) {
  clear();

  trieSize = acReader.nextInt();
  for (int i = 0; i < trieSize; i++) {
    edgeId.emplace_back(acReader.nextInt());
    father.emplace_back(acReader.nextInt());
    next.emplace_back(acReader.nextInt());
    frequency.emplace_back(acReader.nextInt());
    int childrenSize = acReader.nextInt();
    std::unordered_map<int, int> children;
    trie.emplace_back(children);
    for (int j = 0; j < childrenSize; j++) {
      auto key = acReader.nextInt();
      trie.at(i)[key] = acReader.nextInt();
    }
  }
}

void ACAutomaton::clear() {
  trie.clear();
  edgeId.clear();
  father.clear();
  next.clear();
  frequency.clear();
  trieSize = 0;
}

// Print for debug.
void ACAutomaton::print() const {
  std::cout << "Trie size: " << trieSize << std::endl;
  std::cout << "edge id:";
  for (int i = 0; i < trieSize; i++) {
    std::cout << " " << edgeId.at(i);
  }
  std::cout << std::endl;
  std::cout << "father:";
  for (int i = 0; i < trieSize; i++) {
    std::cout << " " << father.at(i);
  }
  std::cout << std::endl;
  std::cout << "next:";
  for (int i = 0; i < trieSize; i++) {
    std::cout << " " << next.at(i);
  }
  std::cout << std::endl;
  std::cout << "frequency:";
  for (int i = 0; i < trieSize; i++) {
    std::cout << " " << frequency.at(i);
  }
  std::cout << std::endl;
  std::cout << "trie:" << std::endl;
  for (int i = 0; i < trieSize; i++) {
    std::cout << i << " ->";
    auto& children = trie.at(i);
    for (auto it = children.begin(); it != children.end(); it++) {
      std::cout << " (" << it->first << ", " << it->second << ")";
    }
    std::cout << std::endl;
  }
}

// Get the size of the trie.
size_t ACAutomaton::getTrieSize() const {
  return trieSize;
}

// Given previous Trie matching position, get the next matched trie node.
int ACAutomaton::getMatch(int prevPos, int edgeId) const {
  while (prevPos > ROOT_NODE && trie[prevPos].find(edgeId) == trie[prevPos].end()) {
    prevPos = next[prevPos];
  }
  return trie.at(prevPos).at(edgeId);
}

// Get corresponding edge id of a trie node.
int ACAutomaton::getEdge(int index) const {
  return edgeId.at(index);
}

// Get corresponding frequency of a trie node.
int ACAutomaton::getFrequency(int index) const {
  return frequency.at(index);
}

// Get corresponding fathre trie node index of a trie node.
int ACAutomaton::getFather(int index) const {
  return father.at(index);
}

// Get frequency of all trie nodes.
const std::vector<int>& ACAutomaton::getAllFrequency() const {
  return frequency;
}

ACAutomaton::~ACAutomaton() { }
