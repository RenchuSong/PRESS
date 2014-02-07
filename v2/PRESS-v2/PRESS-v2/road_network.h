//
//  road_network.h
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_road_network_h
#define PRESS_v2_road_network_h

#include <iostream>
#include <vector>
#include "file_processor.h"
#include "utility.h"

class Edge;
class Node;

// Edge of graph
class Edge {
public:
	int id;
	Node *startNode, *endNode;
	int geoSize = 0;
	vector<EcldPoint> geometry;
	double len;
	
	Edge(int id, Node& startNode, Node& endNode) {
		this->id = id;
		this->startNode = &startNode;
		this->endNode = &endNode;
		this->geometry.clear();
	}
	
	void appendGeometry(EcldPoint& point) {
		++this->geoSize;
		this->geometry.push_back(point);
	}
	
	void setLength(double len) {
		this->len = len;
	}
	
	void setLength() {
		if (this->geometry.empty()) {
			throw "Geometry not initialized";
		}
		this->len = 0;
		for (int i = 1; i < geoSize; ++i) {
			this->len += ecldDistance(this->geometry[i - 1], this->geometry[i]);
		}
	}
	
	// display edge structure
	void display() {
		cout << "Edge " << this->id << ":" << endl;
		for (int i = 0; i < this->geoSize; ++i) {
			cout << "(" << geometry[i].x << "," << geometry[i].y << ") ";
		}
		cout << endl << endl;
	}
};

// Node of graph
class Node {
public:
	int id;
	EcldPoint* location = NULL;
	int edgeNumber = 0;
	vector<Edge*> outEdges;
	
	Node(int id, double x, double y) {
		this->id = id;
		this->location = new EcldPoint(x, y);
		this->outEdges.clear();
	}
	
	void appendOutEdge(Edge* e) {
		++ this->edgeNumber;
		this->outEdges.push_back(e);
	}
	
	void display() {
		cout << "Node " << this->id << ":" << endl;
		cout << "location (" << location->x << " " << location-> y << ")" << endl;
		cout << "EdgeList:";
		for (int i = 0; i < edgeNumber; ++i) {
			cout << " " << outEdges[i]->id;
		}
		cout << endl << endl;
	}
};

//Graph
class Graph {
	
};

#endif
