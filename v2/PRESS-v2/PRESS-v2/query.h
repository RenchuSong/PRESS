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
#include "press.h"

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
	
	static double range(Graph* graph, Auxiliary* auxiliary, RoadNetTrajectory* trajectory, double t1, double t2, MBR* range) {
		// Calculate d1 from given t1 at temporal component
		double d1 = 0;
		for (int i = 0; i < trajectory->temporalNumber; ++i) {
			if (trajectory->temporal->at(i)->t > t1) {
				d1 += (double)(t1 - trajectory->temporal->at(i - 1)->t) /
				(trajectory->temporal->at(i)->t - trajectory->temporal->at(i - 1)->t) *
				trajectory->temporal->at(i)->d;
			} else {
				d1 += trajectory->temporal->at(i)->d;
			}
		}
		
		// Calculate d2 from given t2 at temporal component
		double d2 = 0;
		for (int i = 0; i < trajectory->temporalNumber; ++i) {
			if (trajectory->temporal->at(i)->t > t2) {
				d2 += (double)(t2 - trajectory->temporal->at(i - 1)->t) /
				(trajectory->temporal->at(i)->t - trajectory->temporal->at(i - 1)->t) *
				trajectory->temporal->at(i)->d;
			} else {
				d2 += trajectory->temporal->at(i)->d;
			}
		}
		
		// check if the interval of trajectory intersects the range at some point
		double d = 0;
		for (int i = 0; i < trajectory->spatialNumber; ++i) {
			if (d <d2 && d + graph->getEdge(trajectory->spatial->at(i))->len > d1) {
				
				// TODO: precisely cut the edge
				// use interpolation
				
				Node* n1 = graph->getEdge(trajectory->spatial->at(i))->startNode;
				Node* n2 = graph->getEdge(trajectory->spatial->at(i))->endNode;
				if (range->cross(n1->location, n2->location)) return true;
			}
			d += graph->getEdge(trajectory->spatial->at(i))->len;
		}
		
		return false;
	}
	
	// ========== Query on Compressed Trajectory
	static EcldPoint* whereAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Auxiliary* auxiliary, PRESSCompressedTrajectory* trajectory, int t) {
		// Calculate d from given t at temporal component
		double d = 0;
		for (int i = 0; i < trajectory->temporal->size(); ++i) {
			if (trajectory->temporal->at(i)->t > t) {
				d += (double)(t - trajectory->temporal->at(i - 1)->t) /
				(trajectory->temporal->at(i)->t - trajectory->temporal->at(i - 1)->t) *
				trajectory->temporal->at(i)->d;
			} else {
				d += trajectory->temporal->at(i)->d;
			}
		}
		
		// TODO: Get location from spatial component
		vector<int>* fstList = PRESS::FSTComplement(graph, ac, huffman, trajectory->spatial);
		
		throw "timestamp outside trajectory travel period";
	}
	
	static double whenAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Auxiliary* auxiliary, PRESSCompressedTrajectory* trajectory, EcldPoint* queryLocation) {
		
		return 0;
	}
	
	static double rangeOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Auxiliary* auxiliary, PRESSCompressedTrajectory* trajectory, double t1, double t2, MBR* range) {
		
		return false;
	}
};

#endif
