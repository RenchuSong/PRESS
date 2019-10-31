//
//  auxiliaries.hpp
//  press-v3
//
//  Created by Renchu Song on 10/30/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef auxiliaries_hpp
#define auxiliaries_hpp

#include <iostream>

// A 2D point.
struct Point2D {
  double x;
  double y;
  
  Point2D(double x, double y): x(x), y(y) { }
  void print() {
    std::cout << "(" << x << ", " << y << ")";
  }
};

#endif /* auxiliaries_hpp */