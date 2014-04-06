//
//  query.h
//  PRESS-v2
//
//  Created by Renchu Song on 6/4/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_query_h
#define PRESS_v2_query_h

#include "road_network.h"
#include "ac_automaton.h"
#include "huffman.h"
#include "trajectory.h"

using namespace std;

class Query {
public:
	static GPSPoint* whereAt(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, int t) {
		
		GPSPoint* result = new GPSPoint(t, 0, 0);
		return result;
	}
};

#endif
