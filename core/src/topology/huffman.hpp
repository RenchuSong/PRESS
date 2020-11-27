//
//  huffman.hpp
//  press-v3
//
//  Created by Renchu Song on 6/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef huffman_hpp
#define huffman_hpp

#include <functional>
#include <iostream>
#include <vector>

#include "./ac_automaton.hpp"
#include "../io/binary.hpp"

const int DECODE_FINISH = -1;

class Huffman {
private:
  // Huffman tree size.
  size_t hmSize;
  // AC automaton size.
  size_t acSize;
  // Positive: left and right child is a huffman node. Negative: an AC automaton node.
  std::vector<std::pair<int, int> > huffman;
  // Huffman code of each AC automaton node.
  std::vector<std::vector<bool> > hmCode;
  // Collect huffman code of each AC automaton node.
  void collectHmCode(int index, std::vector<bool>& code);
  // Comparator for sorting AC automaton nodes frequency.
  std::function<bool(const int left, const int right)> acNodeCmp(const ACAutomaton& acAutomaton);
public:
  Huffman();
  // Read huffman encoding from file.
  Huffman(FileReader& hmReader);
  // Construct huffman encoding of each node of AC automaton.
  Huffman(const ACAutomaton& acAutomaton);
  // Store the huffman encoding to file.
  void store(FileWriter& hmWriter);
  void load(FileReader& hmReader);
  void build(const ACAutomaton& acAutomaton);
  void clear();
  // Print for debug.
  void print() const;
  // Encode an AC automaton node sequence to binary.
  void encode(const std::vector<int>& spatial, std::vector<bool>& binary) const;
  // Decode binary to AC automaton node sequence.
  void decode(const std::vector<bool>& binary, std::vector<int>& spatial) const;
  // Decode the next trie node. Return DECODE_FINISH if nothing to decode.
  int decodeNext(const Binary& binary, int& idx) const;

  ~Huffman();
};

#endif /* huffman_hpp */
