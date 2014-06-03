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
#include "auxiliary.h"

using namespace std;

class Query {
public:
	static GPSPoint* whereAt(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, int t) {
		
		GPSPoint* result = new GPSPoint(t, 0, 0);
		return result;
	}
	
	static double whenAt(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, GPSPoint* queryLocation) {
		
		return 0;
	}
	
	static double range(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
};

#endif
