//
//  huffman.hpp
//  press-v3
//
//  Created by Renchu Song on 6/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef huffman_hpp
#define huffman_hpp

#include <iostream>
#include <vector>

#include "./ac_automaton.hpp"
#include "../io/binary.hpp"

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
  std::function<bool(const int left, const int right)> acNodeCmp(ACAutomaton& acAutomaton);
public:
  // Read huffman encoding from file.
  Huffman(FileReader& hmReader);
  // Construct huffman encoding of each node of AC automaton.
  Huffman(ACAutomaton& acAutomaton);
  // Store the huffman encoding to file.
  void store(FileWriter& hmWriter);
  // Print for debug.
  void print();
  // Encode an AC automaton node sequence to binary.
  void encode(const std::vector<int>& spatial, std::vector<bool>& binary);
  // Decode binary to AC automaton node sequence.
  void decode(std::vector<bool>& binary, std::vector<int>& spatial);

  ~Huffman();
};

#endif /* huffman_hpp */
