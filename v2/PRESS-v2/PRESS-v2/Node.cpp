//
//  Node.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 7/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include "Node.h"
#include "Edge.h"
#include "utility.h"
#include <vector>
#include <iostream>

using namespace std;

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
