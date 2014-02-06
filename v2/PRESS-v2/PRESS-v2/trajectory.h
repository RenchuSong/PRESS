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
			GPSPoint point(fr->nextInt(), fr->nextDouble(), fr->nextDouble());
			this->sequence.push_back(point);
		}
	}
	
	//Store a GPS trajectory
	void store(FileWriter* fw) {
		fw->writeInt(this->pointNumber);
		for (int i = 0; i < this->pointNumber; ++i) {
			fw->writeInt(sequence[i].t);
			fw->writeDouble(sequence[i].x);
			fw->writeDouble(sequence[i].y);
		}
	}
	
	//Display a GPS trajectory
	void display() {
		cout << this->pointNumber << endl;
		for (int i = 0; i < this->pointNumber; ++i) {
			cout << sequence[i].t << ": " << sequence[i].x << " " << sequence[i].y << endl;
		}
	}
};

// Road network trajectory in PRESS
//Temporal representation in (d, t) pair
struct TemporalPair {
	double d;
	int t;
	TemporalPair(int t, double d) {
		this->t = t;
		this->d = d;
	}
};

class RoadNetTrajectory {
public:
	int spatialNumber, temporalNumber;
	vector<int> spatial;
	vector<TemporalPair> temporal;
	
	//Construct a road network trajectory from Spatial File Reader and Temporal File Reader
	RoadNetTrajectory(FileReader* spatialReader, FileReader* temporalReader) {
		//Construct spatial path
		this->spatialNumber = spatialReader->nextInt();
		this->spatial.clear();
		for (int i = 0; i < this->spatialNumber; ++i) {
			this->spatial.push_back(spatialReader->nextInt());
		}
		//Construct temporal sequence
		this->temporalNumber = temporalReader->nextInt();
		this->temporal.clear();
		for (int i = 0; i < this->temporalNumber; ++i) {
			TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextDouble());
			this->temporal.push_back(dtPair);
		}
	}
	
	//Store the road network trajectory
	void store(FileWriter* spatialWriter, FileWriter* temporalWriter) {
		spatialWriter->writeInt(this->spatialNumber);
		for (int i = 0; i < this->spatialNumber; ++i) {
			spatialWriter->writeInt(spatial[i]);
		}
		temporalWriter->writeInt(this->temporalNumber);
		for (int i = 0; i < this->temporalNumber; ++i) {
			temporalWriter->writeInt(temporal[i].t);
			temporalWriter->writeDouble(temporal[i].d);
		}
	}
	
	//Display the road network trajectory
	void display() {
		cout << "Spatial" << endl;
		cout << this->spatialNumber << endl;
		for (int i = 0; i < this->spatialNumber; ++i) {
			cout << this->spatial[i] << " ";
		}
		cout << endl << endl;
		
		cout << "Temporal" << endl;
		cout << this->temporalNumber << endl;
		for (int i = 0; i < this->temporalNumber; ++i) {
			cout << this->temporal[i].t << ", " << this->temporal[i].d << "\t    ";
		}
		cout << endl;
	}
};

#endif
