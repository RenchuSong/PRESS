//
//  binary.cpp
//  press-v3
//
//  Created by Renchu Song on 12/1/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include <cassert>
#include "binary.hpp"

Binary::Binary() { }

// Construct an in-memory binary given the bool array.
Binary::Binary(const std::vector<bool>& binary) {
  setBinary(binary);
}

// Construct an in-memory binary given the length and char array.
Binary::Binary(size_t length, const std::vector<char>& binary) {
  assert (length > (binary.size() - 1) * 8 && length <= binary.size() * 8);
  this->length = length;
  this->binary = binary;
}

// Read a binary from a file.
Binary::Binary(FileReader& binaryReader) {
  length = binaryReader.nextInt();
  auto byteSize = length / 8 + ((length % 8) ? 1 : 0);
  for (auto i = 0; i < byteSize; i++) {
    binary.emplace_back(binaryReader.nextChar());
  }
}

void Binary::setBinary(const std::vector<bool>& binary) {
  length = binary.size();
  this->binary.clear();
  int val = 0;
  for (auto i = 0; i < length; i++) {
    val = val * 2 + binary[i];
    if (i % 8 == 7) {
      this->binary.emplace_back(val);
      val = 0;
    }
  }
  if (length % 8) {
    this->binary.emplace_back(val << (8 - length % 8));
  }
}


// Get binary length.
size_t Binary::getLength() const {
  return length;
}

// Get the binary.
const std::vector<char>& Binary::getBinary() const {
  return binary;
}

// Get the bit array.
void Binary::getBitArray(std::vector<bool>& result) const {
  result.clear();
  for (auto i = 0; i < length; i++) {
    result.push_back(getBitByIndex(i));
  }
}

// Get bit by index.
bool Binary::getBitByIndex(size_t index) const {
  auto byteIndex = index / 8, bitIndex = index % 8;
  assert (byteIndex < length);
  return binary[byteIndex] & (0x80 >> bitIndex);
}

// Store the binary into a file.
void Binary::store(FileWriter& binaryWriter) {
  // Binary can only be stored in binary format file.
  assert (binaryWriter.isBinary());
  binaryWriter.writeInt((int)length);
  for (auto val: binary) {
    binaryWriter.writeChar(val);
  }
}

// Print the binary for debug.
void Binary::print() const {
  std::cout << length << "->";
  for (auto i = 0; i < length; i++) {
    std::cout << " " << getBitByIndex(i);
  }
  std::cout << std::endl;
}

Binary::~Binary() { }
