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
	
public:
	void resetTimer() {
		startTime = clock();
	}
	
	long getSystemClockDuration() {
		return clock() - startTime;
	}
	
	long getMiliSecond() {
		return (clock() - startTime) * 1000 / CLOCKS_PER_SEC;
	}
	
	long getSecond() {
		return (clock() - startTime) / CLOCKS_PER_SEC;
	}
};

#endif
