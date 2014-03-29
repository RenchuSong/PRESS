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
#include <vector>

using namespace std;

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
	void generateRoadNetTrajectory(char* gpsTraPath, char* mapmatchTraPath, FileWriter* spatialWriter, FileWriter* temporalWriter) {
		vector<char*>* gpsPathSet = FileTool::getInstance()->getFileNameSet(gpsTraPath);
		vector<char*>* mapmatchPathSet = FileTool::getInstance()->getFileNameSet(mapmatchTraPath);
		
		
		
		delete gpsPathSet;
		delete mapmatchTraPath;
	}
};

PreProcessor* PreProcessor::instance = NULL;

#endif
