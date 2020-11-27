//
//  timer.cpp
//  press-v3
//
//  Created by Renchu Song on 12/7/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include "timer.hpp"
#include <time.h>
#include <sys/time.h>

long long get_wall_time_usec(){
  struct timeval time;
  if (gettimeofday(&time, NULL)) {
    //  Handle error
    return 0;
  }
  return (long long)((double)time.tv_sec * 1000000 + (double)time.tv_usec);
}

// Start timer.
void Timer::start() {
  reset();
  paused = false;
}

// Reset timer to be 0.
void Timer::reset() {
  startTime = get_wall_time_usec();
  accumulation = 0;
}

// Pause the timer.
void Timer::pause() {
  if (!paused) {
    accumulation += get_wall_time_usec() - startTime;
    paused = true;
  }
}

// Resume the timer.
void Timer::resume() {
  if (paused) {
    paused = false;
    startTime = get_wall_time_usec();
  }
}

// Original time unit (different according to system and compiler, use carefully!)
long long Timer::getSystemClockDuration() const {
  return (paused ? 0 : get_wall_time_usec() - startTime) + accumulation;
}

// Time unit: milisecond
long long Timer::getMilliSeconds() const {
  return getSystemClockDuration() / 1000;
}

// Time unit: second
long long Timer::getSeconds() const {
  return getSystemClockDuration() / 1000000;
}
