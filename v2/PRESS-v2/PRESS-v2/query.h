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
	// ========== Query on Original Trajectory
	static GPSPoint* whereAt(Graph* graph, RoadNetTrajectory* trajectory, int t) {
		// Calculate d from given t at temporal component
		double d = 0;
		for (int i = 0; i < trajectory->temporalNumber; ++i) {
			if (trajectory->temporal->at(i)->t > t) {
				d += (double)(t - trajectory->temporal->at(i - 1)->t) /
				(trajectory->temporal->at(i)->t - trajectory->temporal->at(i - 1)->t) *
				trajectory->temporal->at(i)->d;
			} else {
				d += trajectory->temporal->at(i)->d;
			}
		}
		
		// Get location from spatial component
		for (int i = 0; i < trajectory->spatialNumber; ++i) {
			if (d > trajectory->spatial->at(i)) {
				d -= graph->getEdge(trajectory->spatial->at(i))->len;
			} else {
				Edge* edge = graph->getEdge(trajectory->spatial->at(i));
				double ratio = d / edge->len;
				double x =
				GPSPoint* result = new GPSPoint(t, x, y);
				return result;
			}
		}
	}
	
	static double whenAt(Graph* graph, RoadNetTrajectory* trajectory, GPSPoint* queryLocation) {
		
		return 0;
	}
	
	static double range(Graph* graph, RoadNetTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
	
	// ========== Query on Compressed Trajectory
	static GPSPoint* whereAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, int t) {
		
		GPSPoint* result = new GPSPoint(t, 0, 0);
		return result;
	}
	
	static double whenAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, GPSPoint* queryLocation) {
		
		return 0;
	}
	
	static double rangeOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
};

#endif
