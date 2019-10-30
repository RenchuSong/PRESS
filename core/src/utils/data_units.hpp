//
//  data_units.hpp
//  press-v3
//
//  Created by Renchu Song on 10/28/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef data_units_hpp
#define data_units_hpp

// Convert between char and binary.
union Char2Binary {
  char value;
  unsigned char data[0];
};
union Char2Binary char2Binary;

// Convert between short and binary.
union Short2Binary {
  short value;
  unsigned char data[2];
};
union Short2Binary short2Binary;

// Convert between int and binary.
union Int2Binary {
  int value;
  unsigned char data[4];
};
union Int2Binary int2Binary;

// Convert between long and binary.
union Long2Binary {
  long long value;
  unsigned char data[8];
};
union Long2Binary long2Binary;

// Convert between float and binary.
union Float2Binary {
  double value;
  unsigned char data[8];
};
union Float2Binary float2Binary;

// Convert between double and binary.
union Double2Binary {
  double value;
  unsigned char data[8];
};
union Double2Binary double2Binary;

#endif /* data_units_hpp */
