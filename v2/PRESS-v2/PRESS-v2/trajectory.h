//
//  trajectory.h
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_trajectory_h
#define PRESS_v2_trajectory_h

#include <iostream>
#include <vector>
#include "file_processor.h"
using namespace std;

// GPS point is tuple (t, x, y)
struct GPSPoint {
	double x, y;
	int t;
	GPSPoint(int t, double x, double y) {
		this->x = x;
		this->y = y;
		this->t = t;
	}
};

// GPS trajectory is a sequence of GPS points
class GPSTrajectory {
public:
	int pointNumber = 0;
	vector<GPSPoint> sequence;
	
	//Construct a GPS trajectory from a file handler
	GPSTrajectory(FileReader* fr) {
		this->pointNumber = fr->nextInt();
		this->sequence.clear();
		for (int i = 0; i < this->pointNumber; ++i) {
			GPSPoint p(fr->nextInt(), fr->nextDouble(), fr->nextDouble());
			this->sequence.push_back(p);
		}
	}
	
	/* The way to print the GPS trajectory
	void printTrajectory() {
		cout << this->pointNumber << endl;
		for (int i = 0; i < this->pointNumber; ++i) {
			cout << sequence[i].t << " " << sequence[i].x << " " << sequence[i].y << endl;
		}
	}*/
};

#endif
