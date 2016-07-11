//
//  bitvector.cpp
//  press
//
//  Created by Renchu Song on 10/7/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#include <iostream>
#include <cassert>

#include "bitvector.h"

// Construct an empty bit vector
BitVector::BitVector () { }

// Construct a bit vector from vector of bits
BitVector::BitVector (std::vector<bool> data) {
  this->data = data;
}

// Construct a bit vector by reading from a file reader
BitVector::BitVector (FileReader& fileReader) {
  int bitNumber = fileReader.NextInt();
  int byteNumber = bitNumber / 8;
  int mask = 0x80;
  for (int i = 0; i < byteNumber; ++i) {
    unsigned char byte = fileReader.NextChar();
    for (int j = 0; j < 8; ++j) {
      data.push_back(byte & (mask >> j));
    }
  }
  int tailNumber = bitNumber % 8;
  if (tailNumber > 0) {
    unsigned char byte = fileReader.NextChar();
    for (int j = 0; j < tailNumber; ++j) {
      data.push_back(byte & (mask >> j));
    }
  }
}

// Append one bit to the bit vector
void BitVector::Append (bool bit) {
  data.push_back(bit);
}

// Store the bit vector to a FileWriter
void BitVector::Store (FileWriter& fileWriter) {
  int bitNumber = (int)data.size();
  fileWriter.WriteInt(bitNumber);
  int byte = 0;
  for (int i = 0; i < bitNumber; ++i) {
    byte = (byte << 1) + data.at(i);
    if (i % 8 == 7) {
      fileWriter.WriteChar((unsigned char) byte);
      byte = 0;
    }
  }
  if (bitNumber % 8) {
    byte <<= 8 - bitNumber % 8;
    fileWriter.WriteChar((unsigned char) byte);
  }
}

// Show the bit vector in 01 sequence
void BitVector::Show () {
  std::cout << "Bit Vector Size: " << data.size() << std::endl;
  for (int i = 0; i < data.size(); ++i) {
    std::cout << data.at(i);
  }
  std::cout << std::endl;
}

// Get number of bits in the bit vector
size_t BitVector::BitNumber () {
  return data.size();
}

// Get index-th bit in the bit vector
bool BitVector::Get (int index) {
  assert (index < data.size());
  return data.at(index);
}
