//
//  pre_processing.h
//  PRESS-v2
//
//  Created by Renchu Song on 29/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_pre_processing_h
#define PRESS_v2_pre_processing_h

#include "file_processor.h"
#include "road_network.h"
#include "trajectory.h"
#include "press.h"
#include <vector>

using namespace std;

// Point map result is tuple (t, edgeId)
struct PointMatch {
	int t, edgeId;
	PointMatch(int t, int edgeId) {
		this->t = t;
		this->edgeId = edgeId;
	}
};

// Map match result sequence
class MapMatchResult {
public:
	vector<PointMatch*>* sequence = new vector<PointMatch*>();
	
	//Construct a Map Match result from a file handler
	MapMatchResult(FileReader* fr) {
		this->sequence->clear();
		int t;
		while ((t = fr->nextInt()) != EOF) {
			fr->nextChar();
			int edgeId = fr->nextInt();
			fr->nextChar();
			fr->nextDouble();
			this->sequence->push_back(new PointMatch(t, edgeId));
		}
	}
	
	// get processed edge sequence
	vector<int>* getProcessedSequence() {
		vector<int>* result = new vector<int>();
		int i = 0;
		while (sequence->at(i)->edgeId == Config::NULL_POINTER && i++ < sequence->size()) { }
		for (; i < sequence->size(); ++i) {
			if (sequence->at(i)->edgeId != Config::NULL_POINTER) {
				if (!result->size() || result->at(result->size() - 1) != sequence->at(i)->edgeId) {
					result->push_back(sequence->at(i)->edgeId);
				}
			}
		}
		return result;
	}
	
	//Display a GPS trajectory
	void display() {
		for (int i = 0; i < sequence->size(); ++i) {
			cout << sequence->at(i)->t << ": " << sequence->at(i)->edgeId << endl;
		}
	}
	
	~MapMatchResult() {
		for (int i = 0; i < sequence->size(); ++i) {
			delete sequence->at(i);
		}
		delete sequence;
	}
};


class PreProcessor {
private:
	static PreProcessor* instance;
	
public:
	static PreProcessor* getInstance() {
		if (instance == NULL) {
			instance = new PreProcessor();
		}
		return instance;
	}
	
	// generate spatial
	// get GPS path set of one day from gpsTraPath, get map match result from mapmatchTraSet
	// write Spatial and Temporal component to spatialWriter and temporalWriter
	void generateRoadNetTrajectory(Graph* graph, char* gpsTraPath, char* mapmatchTraPath, FileWriter* spatialWriter, FileWriter* temporalWriter) {
		vector<char*>* mapmatchPathSet = FileTool::getInstance()->getFileNameSet(mapmatchTraPath);
		vector<char*>* gpsPathSet = FileTool::getInstance()->getFileNameSet(gpsTraPath);
		
		if (gpsPathSet->size() != mapmatchPathSet->size()) {
			throw "GPS trajectory set number and map matched trajectory set number dont match";
		}
		
		spatialWriter->writeInt((int)gpsPathSet->size());
		temporalWriter->writeInt((int)mapmatchPathSet->size());
		
		for (int i = 0; i < gpsPathSet->size(); ++i) {
			FileReader* gpsReader = new FileReader(gpsPathSet->at(i), false);
			GPSTrajectory* gps = new GPSTrajectory(gpsReader);
			delete gpsReader;
			FileReader* mapReader = new FileReader(mapmatchPathSet->at(i), false);
			MapMatchResult* mm = new MapMatchResult(mapReader);
			delete mapReader;

			// generate spatial component
			vector<int>* spatial = PRESS::SPComplement(graph, mm->getProcessedSequence());

			// generate temporal component
			cout << "herre" << endl;
			

		}
		
		delete gpsPathSet;
		delete mapmatchPathSet;
	}
};

PreProcessor* PreProcessor::instance = NULL;

#endif
