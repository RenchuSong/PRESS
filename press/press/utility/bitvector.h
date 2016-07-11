//
//  bitvector.h
//  press
//
//  Created by Renchu Song on 10/7/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#ifndef bitvector_h
#define bitvector_h

#include <vector>

#include "filereader.h"
#include "filewriter.h"

/**
 * The class wrapper for an array of bits
 * - data:                            the container of the saved bits
 *
 * - BitVector ():                    Construct empty bit vector
 * - BitVector (vector<bool> data):   Construct bit vector from vector of bits
 * - BitVector (FilerReader& fr):     Construct bit vector from reading a file
 * - Append (bool bit):               Append one bit to the bit vector
 * - Store (FileWriter& fw):          Store the bit vector to file writer
 * - Show ():                         Display the bit vector in 01 sequence
 * - BitNumber ():                    Get number of bits in the bit vector
 * - Get (int index):                 Get the index-th bit in the bit vector
 */
class BitVector {
private:
  std::vector<bool> data;
  
public:
  BitVector ();
  BitVector (std::vector<bool> data);
  BitVector (FileReader& fileReader);
  void Append (bool bit);
  void Store (FileWriter& fileWriter);
  void Show ();
  size_t BitNumber();
  bool Get(int index);
};

#endif /* bitvector_h */
