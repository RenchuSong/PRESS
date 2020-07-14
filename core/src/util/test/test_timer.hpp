//
//  test_timer.hpp
//  press-v3
//
//  Created by Renchu Song on 7/13/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#ifndef test_timer_hpp
#define test_timer_hpp

#include "gtest/gtest.h"
#include <thread>
#include <chrono>

#include "../timer.hpp"

void wasteTime() {
  for (auto i = 0; i < 100000000; i++) { }
}

TEST(TimerTest, StartTest) {
  Timer timer;
  timer.start();
  EXPECT_EQ(0, timer.getMilliSeconds());
  EXPECT_EQ(0, timer.getSeconds());
  EXPECT_LT(0, timer.getSystemClockDuration());
}

TEST(TimerTest, ResetTest) {
  Timer timer;
  timer.start();
  wasteTime();
  EXPECT_LT(0, timer.getMilliSeconds());
  timer.reset();
  EXPECT_EQ(0, timer.getMilliSeconds());
}

TEST(TimerTest, PauseTest) {
  Timer timer;
  timer.start();
  wasteTime();
  auto t1 = timer.getMilliSeconds();
  timer.pause();
  wasteTime();
  auto t2 = timer.getMilliSeconds();
  EXPECT_EQ(t1, t2);
}

TEST(TimerTest, ResumeTest) {
  Timer timer;
  timer.start();
  wasteTime();
  timer.pause();
  wasteTime();
  auto t1 = timer.getMilliSeconds();
  timer.resume();
  wasteTime();
  auto t2 = timer.getMilliSeconds();
  EXPECT_LT(t1, t2);
}

#endif /* test_timer_hpp */
