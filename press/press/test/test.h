//
//  test.h
//  press
//
//  Created by Renchu Song on 28/6/2016.
//  Copyright © 2016 Renchu Song. All rights reserved.
//

#ifndef test_h
#define test_h

class TestBase {
public:
  virtual void Prepare() = 0;
  virtual void Run() = 0;
  virtual void Compare() = 0;
};

class TestUtilityFileReader : public TestBase {};

class TestSub {
  
};

#endif /* test_h */
