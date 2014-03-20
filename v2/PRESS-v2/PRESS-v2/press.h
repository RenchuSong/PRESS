//
//  press.h
//  PRESS-v2
//
//  Created by Renchu Song on 19/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_press_h
#define PRESS_v2_press_h

#include "road_network.h"
#include <vector>

using namespace std;

class PRESS {
public:
	// ========= HSC ==========
	// SP compress, get a compressed spatial component from an original spatial component
	static vector<int>* SPCompression(Graph* graph, vector<int>* source) {
		if (source == NULL || source->size() < 1) {
			throw "Spatial component empty";
		}
		vector<int>* result = new vector<int>();
		result->push_back(source->at(0));
		int preEdge = source->at(0);
		int pointNumber = (int)source->size();
		for (int i = 2; i < pointNumber; ++i) {
			if (SPTable::getPre(graph, preEdge, source->at(i)) != source->at(i - 1)) {
				result->push_back(source->at(i - 1));
				preEdge = source->at(i - 1);
			}
		}
		if (pointNumber > 1) {
			result->push_back(source->at(pointNumber - 1));
		}
		return result;
	}
};

#endif
