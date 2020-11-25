//
//  huffman.cpp
//  press-v3
//
//  Created by Renchu Song on 6/27/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include <algorithm>

#include "huffman.hpp"

Huffman::Huffman() { }

Huffman::Huffman(FileReader& hmReader) {
  load(hmReader);
}

/**
 * hmReader format:
 * n(huffman tree size) m(ac automaton size)
 * node_0_left_index(or ac automaton node index) node_0_right_index(or ac automaton node index)
 * node_1_left_index(or ac automaton node index) node_1_right_index(or ac automaton node index)
 * ...
 * node_(n-1)_left_index(or ac automaton node index) node_(n-1)_right_index(or ac automaton node index)
 */
void Huffman::load(FileReader& hmReader) {
  clear();

  hmSize = hmReader.nextInt();
  acSize = hmReader.nextInt();
  for (auto i = 0; i < hmSize; i++) {
    huffman.emplace_back(std::make_pair(hmReader.nextInt(), hmReader.nextInt()));
  }

  // DFS huffman tree to collect the encoding of all AC automaton nodes.
  hmCode.insert(hmCode.begin(), acSize, std::vector<bool>());
  std::vector<bool> code;
  collectHmCode(0, code);
}

Huffman::Huffman(const ACAutomaton& acAutomaton) {
  build(acAutomaton);
}

// Construct huffman encoding of each node of AC automaton.
void Huffman::build(const ACAutomaton& acAutomaton) {
  clear();

  acSize = acAutomaton.getTrieSize();

  // Sort AC automaton nodes indices based on frequency ascendingly. Root node will be skipped.
  std::vector<int> acIndices;
  for (auto i = 1; i < acSize; i++) {
    acIndices.emplace_back(i);
  }
  std::sort(acIndices.begin(), acIndices.end(), acNodeCmp(acAutomaton));

  std::vector<int> sumFrequency;
  int acPtr = 0, hmPtr = 0;
  int HUGE_NUMBER = (1 << 30) - 1;
  hmSize = 0;
  // Always pick the minimal two nodes to merge.
  for (auto i = 1; i < acSize - 1; i++) {
    int acFreq1 = (acPtr < (int)acSize - 1) ? acAutomaton.getFrequency(acIndices.at(acPtr)) : HUGE_NUMBER;
    int acFreq2 = (acPtr < (int)acSize - 2) ? acAutomaton.getFrequency(acIndices.at(acPtr + 1)) : HUGE_NUMBER;
    int hmFreq1 = (hmPtr < (int)hmSize) ? sumFrequency.at(hmPtr) : HUGE_NUMBER;
    int hmFreq2 = (hmPtr < (int)hmSize - 1) ? sumFrequency.at(hmPtr + 1) : HUGE_NUMBER;
    // Get the minimal operation.
    int operation = 1;
    int cost = acFreq1 + acFreq2;
    if (cost > acFreq1 + hmFreq1) {
      operation = 2;
      cost = acFreq1 + hmFreq1;
    }
    if (cost > hmFreq1 + hmFreq2) {
      operation = 3;
      cost = hmFreq1 + hmFreq2;
    }
    // Take the action.
    sumFrequency.emplace_back(cost);
    hmSize++;
    switch (operation) {
      case 1: {
        // Take 2 AC automaton nodes.
        huffman.emplace_back(std::make_pair(-acIndices.at(acPtr), -acIndices.at(acPtr + 1)));
        acPtr += 2;
        break;
      }
      case 2: {
        // Take 1 AC automaton node and 1 huffman tree internal node.
        huffman.emplace_back(std::make_pair(-acIndices.at(acPtr), hmPtr));
        acPtr++;
        hmPtr++;
        break;
      }
      case 3: {
        // Take 2 huffman tree internal nodes.
        huffman.emplace_back(std::make_pair(hmPtr, hmPtr + 1));
        hmPtr += 2;
        break;
      }
    }
  }
  
  // Reverse the huffman tree.
  for (auto& pair: huffman) {
    if (pair.first >= 0) {
      pair.first = (int)hmSize - pair.first - 1;
    }
    if (pair.second >= 0) {
      pair.second = (int)hmSize- pair.second - 1;
    }
  }
  std::reverse(huffman.begin(), huffman.end());

  // DFS huffman tree to collect the encoding of all AC automaton nodes.
  hmCode.insert(hmCode.begin(), acSize, std::vector<bool>());
  std::vector<bool> code;
  collectHmCode(0, code);
}

// Comparator for sorting AC automaton nodes frequency.
std::function<bool(const int left, const int right)> Huffman::acNodeCmp(
  const ACAutomaton& acAutomaton
) {
  return [acAutomaton](const int left, const int right) {
    return acAutomaton.getFrequency(left) < acAutomaton.getFrequency(right);
  };
}

// Collect the huffman code of each AC automaton node.
void Huffman::collectHmCode(int index, std::vector<bool>& code) {
  if (index < 0) {
    hmCode[-index] = code;
    return;
  }
  code.push_back(false);
  collectHmCode(huffman.at(index).first, code);
  code.pop_back();
  code.push_back(true);
  collectHmCode(huffman.at(index).second, code);
  code.pop_back();
}

// Store the huffman encoding to file.
void Huffman::store(FileWriter& hmWriter) {
  hmWriter.writeInt((int)hmSize);
  hmWriter.writeSeparator();
  hmWriter.writeInt((int)acSize);
  hmWriter.writeEol();
  for (auto i = 0; i < hmSize; i++) {
    hmWriter.writeInt(huffman.at(i).first);
    hmWriter.writeSeparator();
    hmWriter.writeInt(huffman.at(i).second);
    hmWriter.writeEol();
  }
}

void Huffman::clear() {
  hmSize = 0;
  acSize = 0;
  huffman.clear();
  hmCode.clear();
}

// Print the huffman tree for debug.
void Huffman::print() const {
  std::cout << hmSize << " " << acSize << std::endl;
  for (auto i = 0; i < hmSize; i++) {
    std::cout << huffman.at(i).first << " " << huffman.at(i).second << std::endl;
  }
  for (auto i = 0; i < acSize; i++) {
    for (auto c: hmCode[i]) {
      std::cout << c;
    }
    std::cout << std::endl;
  }
}

// Encode an AC automaton node sequence to binary.
void Huffman::encode(const std::vector<int>& spatial, std::vector<bool>& binary) const {
  binary.clear();
  for (auto acNode: spatial) {
    binary.insert(binary.end(), hmCode.at(acNode).begin(), hmCode.at(acNode).end());
  }
}

// Decode binary to AC automaton node sequence.
void Huffman::decode(const std::vector<bool>& binary, std::vector<int>& spatial) const {
  spatial.clear();
  int index = 0;
  for (auto c: binary) {
    index = c ? huffman.at(index).second : huffman.at(index).first;
    if (index < 0) {
      spatial.emplace_back(-index);
      index = 0;
    }
  }
}

// Decode the next trie node. Return DECODE_FINISH if nothing to decode.
int Huffman::decodeNext(const Binary& binary, int& idx) const {
  auto size = binary.getLength();
  int index = 0;
  for (; idx < size; ++idx) {
    auto c = binary.getBitByIndex(idx);
    index = c ? huffman.at(index).second : huffman.at(index).first;
    if (index < 0) {
      ++idx;
      return -index;
    }
  }
  return DECODE_FINISH;
}

Huffman::~Huffman() { }
