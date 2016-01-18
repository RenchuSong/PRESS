//
//  experiment.h
//  PRESS-v2
//
//  Created by Renchu Song on 31/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_experiment_h
#define PRESS_v2_experiment_h

// All experiments should be configed here
#include <iostream>
#include "file_processor.h"
#include "trajectory.h"
#include "road_network.h"
#include "config.h"
#include "utility.h"
#include "press.h"
#include "ac_automaton.h"
#include "huffman.h"
#include "timer.h"
#include "query.h"
#include <vector>

using namespace std;

class Experiment {
private:
	static Experiment* instance;
	
public:
	static Experiment* getInstance() {
		if (instance == NULL) {
			instance = new Experiment();
		}
		return instance;
	}
	
	// Generate SP compressed spatial component from training dataset
	void trainingSPCompress(Graph* graph, char* spatialPath, char* temporalPath, char* compressedPath) {
		
		FileReader* spatial = new FileReader(spatialPath, true);
		FileReader* temporal = new FileReader(temporalPath, true);
		FileWriter* comp = new FileWriter(compressedPath, true);
		
		int t = spatial->nextInt();
		temporal->nextInt();
		comp->writeInt(t);
		for (int i = 0; i < t; ++i) {
			RoadNetTrajectory* tra = new RoadNetTrajectory(spatial, temporal);
			vector<int>* comspatial = PRESS::SPCompression(graph, tra->spatial);
			comp->writeInt((int)comspatial->size());
			for (int j = 0; j < comspatial->size(); ++j) {
				comp->writeInt(comspatial->at(j));
			}
			cout << "Trajectory " << i << " completed" << endl;
		}
		
		delete spatial;
		delete temporal;
		delete comp;
	}
	
	// Generate AC automaton and Huffman tree
	void generateACHuffman(Graph* graph, char* spatialPath, char* temporalPath, char* acPath, int sfxMaxLen, char* huffmanPath) {
		vector<RoadNetTrajectory*>* trajectorySet = new vector<RoadNetTrajectory*>();
		
		FileReader* spatial = new FileReader(spatialPath, true);
		FileReader* temporal = new FileReader(temporalPath, true);
		
		int traNumber = spatial->nextInt();
		temporal->nextInt();
		for (int i = 0; i < traNumber; ++i) {
			trajectorySet->push_back(new RoadNetTrajectory(spatial, temporal));
		}
		
		delete spatial;
		delete temporal;
		
		cout << "AC Automaton Start" << endl;
		ACAutomaton* ac = new ACAutomaton(graph, trajectorySet, sfxMaxLen);
		
		cout << "Huffman Start" << endl;
		HuffmanTree* huffman = new HuffmanTree(ac);
		
		FileWriter* acWriter = new FileWriter(acPath, true);
		ac->store(acWriter);
		delete acWriter;
		
		FileWriter* huffmanWriter = new FileWriter(huffmanPath, true);
		huffman->store(huffmanWriter);
		delete huffmanWriter;
		
		for (int i = 0; i < trajectorySet->size(); ++i) {
			delete trajectorySet->at(i);
		}
		delete trajectorySet;
		
		delete ac;
		delete huffman;
	}
	
	// PRESS compress
	void pressCompress(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, char* spatialPath, char* temporalPath, char* comSpatialPath, char* comTemporalPath, double tsnd, double nstd) {
		FileReader* spatial = new FileReader(spatialPath, true);
		FileReader* temporal = new FileReader(temporalPath, true);
		
		FileWriter* comSpatial = new FileWriter(comSpatialPath, true);
		FileWriter* comTemporal = new FileWriter(comTemporalPath, true);
		int traNumber = spatial->nextInt();
		temporal->nextInt();
		comSpatial->writeInt(traNumber);
		comTemporal->writeInt(traNumber);
		for (int i = 0; i < traNumber; ++i) {
			if (i % 100 == 0) {
				cout << "compressing " << i << endl;
			}
			
			RoadNetTrajectory* trajectory = new RoadNetTrajectory(spatial, temporal);
			PRESSCompressedTrajectory* com = PRESS::compression(graph, ac, huffman, trajectory, tsnd, nstd);
			com->store(comSpatial, comTemporal);
			delete trajectory;
		}
		
		delete comSpatial;
		delete comTemporal;
		delete spatial;
		delete temporal;
	}
	
	// PRESS BTC compression individually
	void btcCompress(Graph* graph, char* spatialPath, char* temporalPath, char* comTemporalPath, double tsnd, double nstd, bool extend) {
		FileReader* spatial = new FileReader(spatialPath, true);
		FileReader* temporal = new FileReader(temporalPath, true);
		
		FileWriter* comTemporal = new FileWriter(comTemporalPath, true);
		int traNumber = spatial->nextInt();
		temporal->nextInt();
		comTemporal->writeInt(traNumber);
		
		for (int i = 0; i < traNumber; ++i) {
			if (i % 100 == 0) {
				cout << "BTC compressing " << i << endl;
			}
			
			RoadNetTrajectory* trajectory = new RoadNetTrajectory(spatial, temporal);
			
			vector<TemporalPair*>* temp = extend ? PRESS::extendBTC(graph, trajectory, tsnd, nstd) : PRESS::basicBTC(trajectory->temporal, tsnd, nstd);
			comTemporal->writeInt((int)temp->size());
			for (int j = 0; j < temp->size(); ++j) {
				comTemporal->writeInt(temp->at(j)->t);
				comTemporal->writeDouble(temp->at(j)->d);
			}
			delete trajectory;
		}
		
		delete comTemporal;
		delete spatial;
		delete temporal;
	}
	
	// merge two spatial/temporal files
	void mergeSpatialOrTemporal(char* file1, char* file2, char* result) {
		FileReader* fr1 = new FileReader(file1, true);
		FileReader* fr2 = new FileReader(file2, true);
		FileWriter* fw = new FileWriter(result, true);
		int traNumber1 = fr1->nextInt();
		int traNumber2 = fr2->nextInt();
		fw->writeInt(traNumber1 + traNumber2);
		for (int i = 0; i < traNumber1; ++i) {
			int tmp = fr1->nextInt();
			fw->writeInt(tmp);
			for (int j = 0; j < tmp; ++j) {
				fw->writeInt(fr1->nextInt());
			}
		}
		for (int i = 0; i < traNumber2; ++i) {
			int tmp = fr2->nextInt();
			fw->writeInt(tmp);
			for (int j = 0; j < tmp; ++j) {
				fw->writeInt(fr2->nextInt());
			}
		}
		delete fr1;
		delete fr2;
		delete fw;
	}
};
Experiment* Experiment::instance = NULL;

#endif
