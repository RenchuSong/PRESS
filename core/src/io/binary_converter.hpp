//
//  binary_converter.hpp
//  press-v3
//
//  Created by Renchu Song on 10/28/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef binary_converter_hpp
#define binary_converter_hpp

// Convert between char and binary.
union Char2Binary {
  char value;
  unsigned char data[0];
};


// Convert between short and binary.
union Short2Binary {
  short value;
  unsigned char data[2];
};

// Convert between int and binary.
union Int2Binary {
  int value;
  unsigned char data[4];
};

// Convert between long and binary.
union Long2Binary {
  long long value;
  unsigned char data[8];
};

// Convert between float and binary.
union Float2Binary {
  float value;
  unsigned char data[4];
};

// Convert between double and binary.
union Double2Binary {
  double value;
  unsigned char data[8];
};

#endif /* binary_converter_hpp */
