//
//  pre_processing.h
//  PRESS-v2
//
//  Created by Renchu Song on 29/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_pre_processing_h
#define PRESS_v2_pre_processing_h

#include "utility.h"
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
		while (i < sequence->size() && sequence->at(i)->edgeId == Config::NULL_POINTER) {
			i++;
		}
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
	void generateRoadNetTrajectory(Graph* graph, char* gpsTraPath, char* mapmatchTraPath, char* spatialPath, char* temporalPath) {
		vector<char*>* mapmatchPathSet = FileTool::getInstance()->getFileNameSet(mapmatchTraPath);
		vector<char*>* gpsPathSet = FileTool::getInstance()->getFileNameSet(gpsTraPath);
		
		if (gpsPathSet->size() != mapmatchPathSet->size()) {
			throw "GPS trajectory set number and map matched trajectory set number dont match";
		}
		
		FileWriter* spatialWriter = new FileWriter(spatialPath, true);
		FileWriter* temporalWriter = new FileWriter(temporalPath, true);
		
		spatialWriter->writeInt((int)gpsPathSet->size());
		temporalWriter->writeInt((int)mapmatchPathSet->size());
		
		for (int i = 0; i < gpsPathSet->size(); ++i) {
			
			cout << gpsPathSet->at(i) << endl;
			
			FileReader* gpsReader = new FileReader(gpsPathSet->at(i), false);
			GPSTrajectory* gps = new GPSTrajectory(gpsReader);
			delete gpsReader;
			FileReader* mapReader = new FileReader(mapmatchPathSet->at(i), false);
			MapMatchResult* mm = new MapMatchResult(mapReader);
			delete mapReader;

			// Hook: outlier trajectory, directly match to 0
			vector<int>* processedSequence = mm->getProcessedSequence();
			if (!processedSequence->size()) {
				processedSequence->push_back(0);
				mm->sequence->at(0)->edgeId = 0;
			}
			
			// generate spatial component
			vector<int>* spatial = PRESS::SPComplement(graph, processedSequence);

			// generate temporal component
			for (int j = 1; j < mm->sequence->size(); ++j) {
				if (mm->sequence->at(j)->edgeId == Config::NULL_POINTER) {
					mm->sequence->at(j)->edgeId = mm->sequence->at(j - 1)->edgeId;
				}
			}
			
			for (int j = (int)mm->sequence->size() - 2; j >= 0; --j) {
				if (mm->sequence->at(j)->edgeId == Config::NULL_POINTER) {
					mm->sequence->at(j)->edgeId = mm->sequence->at(j + 1)->edgeId;
				}
			}
			
			vector<TemporalPair*>* temporal = new vector<TemporalPair*>();
			double accumulate = 0;			// distance accumulation
			int pt = 0;						// pointer to the edge
			for (int j = 0; j < mm->sequence->size(); ++j) {
				while (mm->sequence->at(j)->edgeId != spatial->at(pt)) {
					accumulate += graph->getEdge(spatial->at(pt))->len;
					++pt;
				}
				
				EcldPoint* p = new EcldPoint(gps->sequence->at(j)->x, gps->sequence->at(j)->y);
				double bias = edgeBias(p, graph->getEdge(spatial->at(pt))->geometry);
				delete p;
				temporal->push_back(new TemporalPair(gps->sequence->at(j)->t, accumulate + bias));
			}
			
			// decline gaussian random route
			for (int j = 1; j < temporal->size(); ++j) {
				if (temporal->at(j)->d < temporal->at(j - 1)->d) {
					temporal->at(j)->d = temporal->at(j - 1)->d;
				}
			}
			
			RoadNetTrajectory* trajectory = new RoadNetTrajectory(spatial, temporal);
			trajectory->store(spatialWriter, temporalWriter);
			delete trajectory;
		}
		
		delete gpsPathSet;
		delete mapmatchPathSet;
		delete spatialWriter;
		delete temporalWriter;
	}
};

PreProcessor* PreProcessor::instance = NULL;

#endif
