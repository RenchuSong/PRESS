//
//  timer.hpp
//  press-v3
//
//  Created by Renchu Song on 12/7/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#ifndef timer_hpp
#define timer_hpp

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
  long long getSystemClockDuration() const;
  long long getMilliSeconds() const;
  long long getSeconds() const;
};

#endif /* timer_hpp */
