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
	
	void display() {
		cout << t << ":" << x << " " << y << endl;
	}
};

// GPS trajectory is a sequence of GPS points
class GPSTrajectory {
public:
	vector<GPSPoint*>* sequence = new vector<GPSPoint*>();
	
	//Construct a GPS trajectory from a file handler
	GPSTrajectory(FileReader* fr) {
		this->sequence->clear();
		int t;
		while ((t = fr->nextInt()) != EOF) {
			fr->nextChar();
			double x = fr->nextDouble();
			fr->nextChar();
			double y = fr->nextDouble();
			this->sequence->push_back(new GPSPoint(t, x, y));
		}
	}
	
	//Store a GPS trajectory
	void store(FileWriter* fw) {
		if (!fw->isBinary()) {
			fw->writeChar('\n');
		}
		for (int i = 0; i < sequence->size(); ++i) {
			if (fw->isBinary()) {
				fw->writeInt(sequence->at(i)->t);
				fw->writeDouble(sequence->at(i)->x);
				fw->writeDouble(sequence->at(i)->y);
			} else {
				fw->writeInt(sequence->at(i)->t);
				fw->writeChar(' ');
				fw->writeDouble(sequence->at(i)->x);
				fw->writeChar(' ');
				fw->writeDouble(sequence->at(i)->y);
				fw->writeChar('\n');
			}
		}
	}
	
	//Display a GPS trajectory
	void display() {
		for (int i = 0; i < sequence->size(); ++i) {
			cout << sequence->at(i)->t << ": " << sequence->at(i)->x << " " << sequence->at(i)->y << endl;
		}
	}
	
	~GPSTrajectory() {
		for (int i = 0; i < sequence->size(); ++i) {
			delete sequence->at(i);
		}
		delete sequence;
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
	
	void display() {
		cout << t << ":" << d << endl;
	}
};

class RoadNetTrajectory {
public:
	int spatialNumber, temporalNumber;
	vector<int>* spatial = new vector<int>();
	vector<TemporalPair*>* temporal = new vector<TemporalPair*>();
	
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
			TemporalPair* dtPair = new TemporalPair(temporalReader->nextInt(), temporalReader->nextDouble());
			this->temporal->push_back(dtPair);
		}
	}
	
	//Construct a trajectory from spatial and temporal vector
	RoadNetTrajectory(vector<int>* spatial, vector<TemporalPair*>* temporal) {
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
			temporalWriter->writeInt(temporal->at(i)->t);
			temporalWriter->writeDouble(temporal->at(i)->d);
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
			cout << this->temporal->at(i)->t << ", " << this->temporal->at(i)->d << "\t    ";
		}
		cout << endl;
	}
	
	~RoadNetTrajectory() {
		delete spatial;
		for (int i = 0; i < temporal->size(); ++i) {
			delete temporal->at(i);
		}
		delete temporal;
	}
};

// Trajectory compressed by PRESS
class PRESSCompressedTrajectory {
public:
	Binary* spatial;
	vector<TemporalPair*>* temporal;
	
	// Construct a PRESSCompressedTrajectory from spatial and temporal component
	PRESSCompressedTrajectory(Binary* binary, vector<TemporalPair*>* temporal) {
		this->spatial = binary;
		this->temporal = temporal;
	}
	
	// Construct a PRESSCompressedTrajectory from FileReaders
	PRESSCompressedTrajectory(FileReader* spatialReader, FileReader* temporalReader) {
		spatial = new Binary(spatialReader);
		int temporalNumber = temporalReader->nextInt();
		this->temporal = new vector<TemporalPair*>();
		this->temporal->clear();
		for (int i = 0; i < temporalNumber; ++i) {
			//TemporalPair dtPair(temporalReader->nextInt(), temporalReader->nextDouble());
			TemporalPair* dtPair = new TemporalPair(temporalReader->nextInt(), temporalReader->nextInt());
			this->temporal->push_back(dtPair);
		}
	}
	
	//Store the PRESS compressed trajectory
	void store(FileWriter* spatialWriter, FileWriter* temporalWriter) {
		spatial->store(spatialWriter);
		temporalWriter->writeInt((int)temporal->size());
		for (int i = 0; i < temporal->size(); ++i) {
			temporalWriter->writeInt(temporal->at(i)->t);
			temporalWriter->writeDouble(temporal->at(i)->d);
		}
	}
	
	~PRESSCompressedTrajectory() {
		delete spatial;
		for (int i = 0; i < temporal->size(); ++i) {
			delete temporal->at(i);
		}
		delete temporal;
	}
};
#endif
