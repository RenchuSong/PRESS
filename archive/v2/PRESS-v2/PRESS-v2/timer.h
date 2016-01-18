//
//  timer.h
//  PRESS-v2
//
//  Created by Renchu Song on 26/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_timer_h
#define PRESS_v2_timer_h

#include <ctime>

// Timer class is to get the time consumption of programs
class Timer {
private:
	long startTime;
	long accumulation;
	bool paused;
	
public:
	
	// reset
	void resetTimer() {
		startTime = clock();
		accumulation = 0;
		paused = false;
	}
	
	// pause (to wait for time consuming procedure that should not be accumulated in time consumption)
	void pauseTimer() {
		accumulation += clock() - startTime;
		paused = true;
	}
	
	// resume timer
	void resumeTimer() {
		startTime = clock();
		paused = false;
	}
	
	// original time unit (different according to system and compiler, use carefully!)
	long getSystemClockDuration() {
		if (paused) {
			throw "Timer paused, please resume timer first!";
		}
		return clock() - startTime + accumulation;
	}
	
	// time unit: milisecond
	long getMiliSecond() {
		if (paused) {
			throw "Timer paused, please resume timer first!";
		}
		return (clock() - startTime + accumulation) * 1000 / CLOCKS_PER_SEC;
	}
	
	// time unit: second
	long getSecond() {
		if (paused) {
			throw "Timer paused, please resume timer first!";
		}
		return (clock() - startTime + accumulation) / CLOCKS_PER_SEC;
	}
};

#endif
