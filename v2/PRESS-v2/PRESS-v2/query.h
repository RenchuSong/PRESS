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
#include "utility.h"

using namespace std;

class Query {
public:
	// ========== Query on Original Trajectory
	static EcldPoint* whereAt(Graph* graph, RoadNetTrajectory* trajectory, int t) {
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
				EcldPoint* start = edge->startNode->location;
				EcldPoint* end = edge->endNode->location;
				EcldPoint* result = new EcldPoint(interpolate(start->x, end->x, ratio), interpolate(start->y, end->y, ratio));
				return result;
			}
		}
		throw "timestamp outside trajectory travel period";
	}
	
	static double whenAt(Graph* graph, Auxiliary* auxiliary, RoadNetTrajectory* trajectory, EcldPoint* queryLocation) {
		// Calculate the distance from start of the trajectory to the location
		double d = 0;
		for (int i = 0; i < trajectory->spatialNumber; ++i) {
			bool flag = false;
			double mid = 0;
			Edge* edge = graph->getEdge(trajectory->spatial->at(i));
			if (auxiliary->spMBR[edge->startNode->id][edge->endNode->id]->contain(queryLocation)) {
				mid = bias(edge->geometry, queryLocation);
				if (mid != Config::NULL_POINTER) {
					flag = true;
				}
			}
			if (!flag) {
				d += edge->len;
			} else {
				d += mid; break;
			}
		}
		
		// Calculate the time stamp
		for (int i = 1; i < trajectory->temporalNumber; ++i) {
			if (trajectory->temporal->at(i - 1)->d <= d && d <= trajectory->temporal->at(i)->d) {
				double r = unInterpolate(
					trajectory->temporal->at(i - 1)->d,
					trajectory->temporal->at(i)->d,
					d);
				return interpolate(trajectory->temporal->at(i - 1)->t, trajectory->temporal->at(i)->t, r);
			}
		}
		
		throw "never at this place";
	}
	
	static double range(Graph* graph, RoadNetTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
	
	// ========== Query on Compressed Trajectory
	static EcldPoint* whereAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, int t) {
		
		EcldPoint* result = new EcldPoint(0, 0);
		return result;
	}
	
	static double whenAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, EcldPoint* queryLocation) {
		
		return 0;
	}
	
	static double rangeOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
};

#endif
