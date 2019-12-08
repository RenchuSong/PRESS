//
//  timer.hpp
//  press-v3
//
//  Created by Renchu Song on 12/7/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

#include <ctime>

class Timer {
private:
  long long startTime;
  long long accumulation;
  bool paused;

public:
  void start();
  void reset();
  void pause();
  void resume();
  long long getSystemClockDuration();
  long long getMilliSeconds();
  long long getSeconds();
};

#endif /* timer_hpp */
