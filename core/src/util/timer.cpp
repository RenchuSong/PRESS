//
//  timer.cpp
//  press-v3
//
//  Created by Renchu Song on 12/7/19.
//  Copyright © 2019 Renc. All rights reserved.
//

#include "timer.hpp"

// Start timer.
void Timer::start() {
  reset();
  paused = false;
}

// Reset timer to be 0.
void Timer::reset() {
  startTime = clock();
  accumulation = 0;
}

// Pause the timer.
void Timer::pause() {
  if (!paused) {
    accumulation += clock() - startTime;
    paused = true;
  }
}

// Resume the timer.
void Timer::resume() {
  if (paused) {
    paused = false;
    startTime = clock();
  }
}

// Original time unit (different according to system and compiler, use carefully!)
long long Timer::getSystemClockDuration() {
  return (paused ? 0 : clock() - startTime) + accumulation;
}

// Time unit: milisecond
long long Timer::getMilliSeconds() {
  return getSystemClockDuration() * 1000 / CLOCKS_PER_SEC;
}

// Time unit: second
long long Timer::getSeconds() {
  return getSystemClockDuration() / CLOCKS_PER_SEC;
}
