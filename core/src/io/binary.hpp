//
//  binary.hpp
//  press-v3
//
//  Created by Renchu Song on 12/1/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef binary_hpp
#define binary_hpp

#include <vector>

#include "./file_reader.hpp"
#include "./file_writer.hpp"

class Binary {
private:
  size_t length;
  std::vector<char> binary;

public:
  Binary();
  Binary(const std::vector<bool>& binary);
  Binary(size_t length, const std::vector<char>& binary);
  Binary(FileReader& binaryReader);
  void setBinary(const std::vector<bool>& binary);
  size_t getLength() const;
  const std::vector<char>& getBinary() const;
  void getBitArray(std::vector<bool>& result) const;
  bool getBitByIndex(size_t index) const;
  void store(FileWriter& binaryWriter);
  void print() const;
  ~Binary();
};

#endif /* binary_hpp */
