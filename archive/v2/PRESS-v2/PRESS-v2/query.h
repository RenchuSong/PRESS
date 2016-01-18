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
	
	static bool range(Graph* graph, Auxiliary* auxiliary, RoadNetTrajectory* trajectory, double t1, double t2, MBR* range) {
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
		if (ac->rootAncestorInit == false) throw "Must first init root ancestor";
		
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
		
		double td = 0;
		vector<int>* fstList = PRESS::FSTComplement(graph, ac, huffman, trajectory->spatial);
		for (int i = 0; i < fstList->size(); ++i) {
			if (i > 0) {
				int l = ac->getNode(fstList->at(i - 1))->rootAncestor;
				int r = ac->getNode(fstList->at(i - 1))->value;
				if (td + auxiliary->spLen[l][r] >= d) {
					vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{l, r});
					for (int j = 1; j < edgeList->size() - 1; ++j) {
						if (td + graph->getEdge(edgeList->at(j))->len < d) {
							td += graph->getEdge(edgeList->at(j))->len;
						} else {
							Edge* edge = graph->getEdge(edgeList->at(j));
							double ratio = (d - td) / edge->len;
							EcldPoint* start = edge->startNode->location;
							EcldPoint* end = edge->endNode->location;
							EcldPoint* result = new EcldPoint(interpolate(start->x, end->x, ratio), interpolate(start->y, end->y, ratio));
							return result;
						}
					}
				}
			}
			if (td + auxiliary->fstLen[fstList->at(i)] >= d) {
				vector<int> fst;
				int node = fstList->at(i);
				while (node != -1) {
					fst.push_back(ac->getNode(node)->value);
					node = ac->getNode(node)->father;
				}
				reverse(fst.begin(), fst.end());
				for (int j = 0; j < fst.size(); ++j) {
					if (td + graph->getEdge(fst[j])->len < d) {
						td += graph->getEdge(fst[j])->len;
					} else {
						Edge* edge = graph->getEdge(fst[j]);
						double ratio = (d - td) / edge->len;
						EcldPoint* start = edge->startNode->location;
						EcldPoint* end = edge->endNode->location;
						EcldPoint* result = new EcldPoint(interpolate(start->x, end->x, ratio), interpolate(start->y, end->y, ratio));
						return result;
					}
					if (j < fst.size() - 1) {
						int l = fst[j];
						int r = fst[j + 1];
						if (td + auxiliary->spLen[l][r] >= d) {
							vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{l, r});
							for (int j = 1; j < edgeList->size() - 1; ++j) {
								if (td + graph->getEdge(edgeList->at(j))->len < d) {
									td += graph->getEdge(edgeList->at(j))->len;
								} else {
									Edge* edge = graph->getEdge(edgeList->at(j));
									double ratio = (d - td) / edge->len;
									EcldPoint* start = edge->startNode->location;
									EcldPoint* end = edge->endNode->location;
									EcldPoint* result = new EcldPoint(interpolate(start->x, end->x, ratio), interpolate(start->y, end->y, ratio));
									return result;
								}
							}
						}
					}
				}
				Edge* edge = graph->getEdge(fst[fst.size() - 1]);
				if (td + edge->len < d) {
					td += edge->len;
				} else {
					double ratio = (d - td) / edge->len;
					EcldPoint* start = edge->startNode->location;
					EcldPoint* end = edge->endNode->location;
					EcldPoint* result = new EcldPoint(interpolate(start->x, end->x, ratio), interpolate(start->y, end->y, ratio));
					return result;
				}
			}
		}
		
		throw "timestamp outside trajectory travel period";
	}
	
	static double whenAtOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Auxiliary* auxiliary, PRESSCompressedTrajectory* trajectory, EcldPoint* queryLocation) {
		vector<int>* fstList = PRESS::FSTComplement(graph, ac, huffman, trajectory->spatial);
		int d = 0;
		bool flag = false;
		for (int i = 0; i < fstList->size(); ++i) {
			// MBR of fst
			if (auxiliary->fstMBR[fstList->at(i)]->contain(queryLocation)) {
				vector<int> fst;
				int node = fstList->at(i);
				while (node != -1) {
					fst.push_back(ac->getNode(node)->value);
					node = ac->getNode(node)->father;
				}
				reverse(fst.begin(), fst.end());
				for (int j = 0; i < fst.size(); ++j) {
					double dist = bias(graph->getEdge(fst[j])->geometry, queryLocation);
					if (dist != Config::NULL_POINTER) {
						d += dist;
						flag = true;
						break;
					} else d += graph->getEdge(fst[j])->len;
					if (j < fst.size() - 1) {
						vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{fst[j], fst[j + 1]});
						for (int k = 1; k < edgeList->size() - 1; ++k) {
							double dist = bias(graph->getEdge(edgeList->at(k))->geometry, queryLocation);
							if (dist != Config::NULL_POINTER) {
								d += dist;
								flag = true;
								break;
							} else d += graph->getEdge(edgeList->at(k))->len;
						}
					}
					if (flag) break;
				}
			}
			if (flag) break;
			// MBR of link SP segment
			if (i > 0 && auxiliary->spMBR[ac->getNode(fstList->at(i - 1))->value][ac->getNode(fstList->at(i))->rootAncestor]->contain(queryLocation)) {
				vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{ac->getNode(fstList->at(i - 1))->value, ac->getNode(fstList->at(i))->rootAncestor});
				for (int k = 1; k < edgeList->size() - 1; ++k) {
					double dist = bias(graph->getEdge(edgeList->at(k))->geometry, queryLocation);
					if (dist != Config::NULL_POINTER) {
						d += dist;
						flag = true;
						break;
					} else d += graph->getEdge(edgeList->at(k))->len;
				}
			}
			if (flag) break;
		}
		
		if (flag) {
			for (int i = 0; i < trajectory->temporal->size() - 1; ++i) {
				if (trajectory->temporal->at(i + 1)->d > d) {
					return interpolate(trajectory->temporal->at(i)->t, trajectory->temporal->at(i + 1)->t, unInterpolate(trajectory->temporal->at(i)->d, trajectory->temporal->at(i + 1)->d, d));
				}
			}
		}
		
		throw "location outside trajectory travel path";
	}
	
	static bool rangeOnCompressed(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Auxiliary* auxiliary, PRESSCompressedTrajectory* trajectory, double t1, double t2, MBR* range) {
		// Calculate d1 from given t1 at temporal component
		double d1 = 0;
		for (int i = 0; i < trajectory->temporal->size(); ++i) {
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
		for (int i = 0; i < trajectory->temporal->size(); ++i) {
			if (trajectory->temporal->at(i)->t > t2) {
				d2 += (double)(t2 - trajectory->temporal->at(i - 1)->t) /
				(trajectory->temporal->at(i)->t - trajectory->temporal->at(i - 1)->t) *
				trajectory->temporal->at(i)->d;
			} else {
				d2 += trajectory->temporal->at(i)->d;
			}
		}
		
		// Scan MBR
		int d = 0;
		vector<int>* fstList = PRESS::FSTComplement(graph, ac, huffman, trajectory->spatial);
		for (int i = 0; i < fstList->size() && d < d2; ++i) {
			
			// TODO: precisely cut the edge
			// use interpolation
			
			// MBR of fst
			if (d + auxiliary->fstLen[fstList->at(i)] > d1 && auxiliary->fstMBR[fstList->at(i)]->intersect(range)) {
				vector<int> fst;
				int node = fstList->at(i);
				while (node != -1) {
					fst.push_back(ac->getNode(node)->value);
					node = ac->getNode(node)->father;
				}
				reverse(fst.begin(), fst.end());
				int acc = 0;
				for (int j = 0; i < fst.size(); ++j) {
					if (d + acc < d2 && d + acc + graph->getEdge(fst[j])->len > d1) {
						Node* n1 = graph->getEdge(fst[j])->startNode;
						Node* n2 = graph->getEdge(fst[j])->endNode;
						if (range->cross(n1->location, n2->location)) return true;
					}
					acc += graph->getEdge(fst[j])->len;
					if (j < fst.size() - 1) {
						vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{fst[j], fst[j + 1]});
						for (int k = 1; k < edgeList->size() - 1; ++k) {
							if (d + acc < d2 && d + acc + graph->getEdge(edgeList->at(k))->len > d1) {
								Node* n1 = graph->getEdge(edgeList->at(k))->startNode;
								Node* n2 = graph->getEdge(edgeList->at(k))->endNode;
								if (range->cross(n1->location, n2->location)) return true;
							}
							acc += graph->getEdge(edgeList->at(k))->len;
						}
					}
				}
			}
			d += auxiliary->fstLen[fstList->at(i)];
			if (i > 0) {
				if (d + auxiliary->spLen[ac->getNode(fstList->at(i - 1))->value][ac->getNode(fstList->at(i))->rootAncestor] > d1 && auxiliary->spMBR[ac->getNode(fstList->at(i - 1))->value][ac->getNode(fstList->at(i))->rootAncestor]->intersect(range)) {
					int acc = 0;
					vector<int>* edgeList = PRESS::SPComplement(graph, new vector<int>{ac->getNode(fstList->at(i - 1))->value, ac->getNode(fstList->at(i))->rootAncestor});
					for (int k = 1; k < edgeList->size() - 1; ++k) {
						if (d + acc < d2 && d + acc + graph->getEdge(edgeList->at(k))->len > d1) {
							Node* n1 = graph->getEdge(edgeList->at(k))->startNode;
							Node* n2 = graph->getEdge(edgeList->at(k))->endNode;
							if (range->cross(n1->location, n2->location)) return true;
						}
						acc += graph->getEdge(edgeList->at(k))->len;
					}
				}
				d += auxiliary->spLen[ac->getNode(fstList->at(i - 1))->value][ac->getNode(fstList->at(i))->rootAncestor];
			}
		}
		
		return false;
	}
};

#endif
