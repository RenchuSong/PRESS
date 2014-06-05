//
//  auxiliary.h
//  PRESS-v2
//
//  Created by Renchu Song on 2/6/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_auxiliary_h
#define PRESS_v2_auxiliary_h

#include "file_processor.h"

// MBR class with four boundaries (x1, x2, y1, y2) defining the rectangle region in 2D euclidean space.
class MBR {
public:
	double x1, x2, y1, y2;		// MBR boundary
	
	// Construct MBR from boundary params
	MBR(double x1, double x2, double y1, double y2) {
		this->x1 = x1;
		this->x2 = x2;
		this->y1 = y1;
		this->y2 = y2;
	}
	
	// load MBR from file
	MBR(FileReader* fr) {
		this->x1 = fr->nextDouble();
		this->x2 = fr->nextDouble();
		this->y1 = fr->nextDouble();
		this->y2 = fr->nextDouble();
	}
	
	// Store MBR to file
	void store(FileWriter* fw) {
		fw->writeDouble(this->x1);
		fw->writeDouble(this->x2);
		fw->writeDouble(this->y1);
		fw->writeDouble(this->y2);
	}
	
	// Judge if two MBR has intersection with each other
	bool intersect(MBR* other) {
		return (this->x1 < other->x2 && this->x2 > other->x1 && this->y1 < other->y2 && this->y2 > other->y1);
	}
};

class Auxiliary {
public:
	int nodeNumber;
	int edgeNumber;
	int fstNumber;
	
	vector<MBR*>* fstMBR;		// The MBR of a sub-trajectory
	vector<MBR*>** spMBR;		// The MBR of a shortest path (between two nodes)
	vector<MBR*>* edgeMBR;		// The MBR of an edge
	
	vector<double>* fstLen;		// The total road network length of a sub-trajectory
	vector<double>** spLen;		// The total road netwoek length of a shortest path (between two nodes)
	
	Auxiliary(int nodeNumber, int edgeNumber, int fstNumber, FileReader* fstMBRReader, FileReader* spMBRReader, FileReader* edgeMBRReader, FileReader* fstLenReader, FileReader* spLenReader) {
		
	}
};

#endif
