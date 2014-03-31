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
		
		ACAutomaton* ac = new ACAutomaton(graph, trajectorySet, sfxMaxLen);
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
	}
	
	
};
Experiment* Experiment::instance = NULL;

#endif
