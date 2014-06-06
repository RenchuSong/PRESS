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
	
	MBR** fstMBR;				// The MBR of a sub-trajectory
	MBR*** spMBR;				// The MBR of a shortest path (between two nodes)
	MBR** edgeMBR;				// The MBR of an edge
	
	double* fstLen;				// The total road network length of a sub-trajectory
	double** spLen;				// The total road netwoek length of a shortest path (between two nodes)
	
	Auxiliary(int nodeNumber, int edgeNumber, int fstNumber, FileReader* fstMBRReader, FileReader* spMBRReader, FileReader* edgeMBRReader, FileReader* fstLenReader, FileReader* spLenReader) {
		this->nodeNumber = nodeNumber;
		this->edgeNumber = edgeNumber;
		this->fstNumber = fstNumber;
		this->fstMBR = new MBR*[fstNumber];
		for (int i = 0; i < this->nodeNumber; ++i) {
			this->fstMBR[i] = new MBR(fstMBRReader);
		}
		this->spMBR = new MBR**[nodeNumber];
		for (int i = 0; i < this->nodeNumber; ++i) {
			this->spMBR[i] = new MBR*[nodeNumber];
			for (int j = 0; j < this->nodeNumber; ++j) {
				this->spMBR[i][j] = new MBR(spMBRReader);
			}
		}
		this->edgeMBR = new MBR*[edgeNumber];
		for (int i = 0; i < this->edgeNumber; ++i) {
			this->edgeMBR[i] = new MBR(edgeMBRReader);
		}
		this->fstLen = new double[fstNumber];
		for (int i = 0; i < this->fstNumber; ++i) {
			this->fstLen[i] = fstLenReader->nextDouble();
		}
		this->spLen = new double*[nodeNumber];
		for (int i = 0; i < this->nodeNumber; ++i) {
			this->spLen[i] = new double[nodeNumber];
			for (int j = 0; j < this->nodeNumber; ++j) {
				this->spLen[i][j] = spLenReader->nextDouble();
			}
		}
	}
};

#endif
