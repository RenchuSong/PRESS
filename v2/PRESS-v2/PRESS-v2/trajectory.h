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
		if (!fw->isBinary()) {
			fw->writeChar('\n');
		}
		for (int i = 0; i < this->pointNumber; ++i) {
			if (fw->isBinary()) {
				fw->writeInt(sequence[i].t);
				fw->writeDouble(sequence[i].x);
				fw->writeDouble(sequence[i].y);
			} else {
				fw->writeInt(sequence[i].t);
				fw->writeChar(' ');
				fw->writeDouble(sequence[i].x);
				fw->writeChar(' ');
				fw->writeDouble(sequence[i].y);
				fw->writeChar('\n');
			}
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
	vector<int>* spatial = new vector<int>();
	vector<TemporalPair>* temporal = new vector<TemporalPair>();
	
	//Construct a road network trajectory from Spatial File Reader and Temporal File Reader
	RoadNetTrajectory(FileReader* spatialReader, FileReader* temporalReader) {
		//Construct spatial path
		this->spatialNumber = spatialReader->nextInt();
		this->spatial->clear();
		for (int i = 0; i < this->spatialNumber; ++i) {
			this->spatial->push_back(spatialReader->nextInt());
		}
		//Construct temporal sequence
		this->temporalNumber = temporalReader->nextInt();
		this->temporal->clear();
		for (int i = 0; i < this->temporalNumber; ++i) {
			//TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextDouble());
			TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextInt());
			this->temporal->push_back(dtPair);
		}
	}
	
	//Construct a trajectory from spatial and temporal vector
	RoadNetTrajectory(vector<int>* spatial, vector<TemporalPair>* temporal) {
		this->spatial = spatial;
		this->spatialNumber = (int)spatial->size();
		this->temporal = temporal;
		this->temporalNumber = (int)temporal->size();
	}
	
	//Store the road network trajectory
	void store(FileWriter* spatialWriter, FileWriter* temporalWriter) {
		spatialWriter->writeInt(this->spatialNumber);
		for (int i = 0; i < this->spatialNumber; ++i) {
			spatialWriter->writeInt(spatial->at(i));
		}
		temporalWriter->writeInt(this->temporalNumber);
		for (int i = 0; i < this->temporalNumber; ++i) {
			temporalWriter->writeInt(temporal->at(i).t);
			temporalWriter->writeDouble(temporal->at(i).d);
		}
	}
	
	//Display the road network trajectory
	void display() {
		cout << "Spatial" << endl;
		cout << this->spatialNumber << endl;
		for (int i = 0; i < this->spatialNumber; ++i) {
			cout << this->spatial->at(i) << " ";
		}
		cout << endl << endl;
		
		cout << "Temporal" << endl;
		cout << this->temporalNumber << endl;
		for (int i = 0; i < this->temporalNumber; ++i) {
			cout << this->temporal->at(i).t << ", " << this->temporal->at(i).d << "\t    ";
		}
		cout << endl;
	}
	
	~RoadNetTrajectory() {
		delete spatial;
		delete temporal;
	}
};

// Trajectory compressed by PRESS
class PRESSCompressedTrajectory {
public:
	Binary* spatial;
	vector<TemporalPair>* temporal;
	
	// Construct a PRESSCompressedTrajectory from spatial and temporal component
	PRESSCompressedTrajectory(Binary* binary, vector<TemporalPair>* temporal) {
		this->spatial = binary;
		this->temporal = temporal;
	}
	
	// Construct a PRESSCompressedTrajectory from FileReaders
	PRESSCompressedTrajectory(FileReader* spatialReader, FileReader* temporalReader) {
		spatial = new Binary(spatialReader);
		int temporalNumber = temporalReader->nextInt();
		this->temporal = new vector<TemporalPair>();
		this->temporal->clear();
		for (int i = 0; i < temporalNumber; ++i) {
			//TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextDouble());
			TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextInt());
			this->temporal->push_back(dtPair);
		}
	}
	
	//Store the PRESS compressed trajectory
	void store(FileWriter* spatialWriter, FileWriter* temporalWriter) {
		spatial->store(spatialWriter);
		temporalWriter->writeInt((int)temporal->size());
		for (int i = 0; i < temporal->size(); ++i) {
			temporalWriter->writeInt(temporal->at(i).t);
			temporalWriter->writeDouble(temporal->at(i).d);
		}
	}
	
	~PRESSCompressedTrajectory() {
		delete spatial;
		delete temporal;
	}
};
#endif
