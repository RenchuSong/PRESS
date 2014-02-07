//
//  Edge.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 7/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include "road_network.h"
#include <iostream>

using namespace std;

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
		cout << "Endpoints: " << this->startNode->id << "->" << this->endNode->id << endl;
		for (int i = 0; i < this->geoSize; ++i) {
			cout << "(" << geometry[i].x << "," << geometry[i].y << ") ";
		}
		cout << endl << endl;
	}
};
