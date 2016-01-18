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
#include <cmath>
#include <vector>
#include <queue>
#include "file_processor.h"
#include "utility.h"
#include "config.h"
#include <sys/stat.h>

using namespace std;

class Node;

// Edge of graph
class Edge {
public:
	int id;												// node id
	Node *startNode, *endNode;							// two endpoints
	int geoSize = 0;									// geometry point number
	vector<EcldPoint*> geometry;						// geometry discription
	double len;
	
	Edge(int id, Node* startNode, Node* endNode) {
		this->id = id;
		this->startNode = startNode;
		this->endNode = endNode;
		this->geometry.clear();
	}
	
	// Add a new geometry point
	void appendGeometry(EcldPoint* point) {
		++this->geoSize;
		this->geometry.push_back(point);
	}
	
	// Set the length of the edge to double value
	void setLength(double len) {
		this->len = len;
	}
	
	// Use geometry to calculate the length of the edge
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
			cout << "(" << geometry[i]->x << "," << geometry[i]->y << ") ";
		}
		cout << "Length: " << this->len << endl;
	}
};

// Node of graph
class Node {
public:
	int id;												// node id
	EcldPoint* location = NULL;							// node location
	int edgeNumber = 0;									// number of edges start from the node
	vector<Edge*> outEdges;								// edges start from the node
	
	Node(int id, double x, double y) {
		this->id = id;
		this->location = new EcldPoint(x, y);
		this->outEdges.clear();
	}
	
	// Add a new edge start from the node
	void appendOutEdge(Edge* e) {
		++this->edgeNumber;
		this->outEdges.push_back(e);
	}
	
	// display the node structure
	void display() {
		cout << "Node " << this->id << ":" << endl;
		cout << "location (" << location->x << " " << location-> y << ")" << endl;
		cout << "Linked edges:" << endl;
		for (int j = 0; j < this->edgeNumber; ++j) {
			cout << this->outEdges[j]->id << " ";
		}
		cout << endl;
	}
	
	// delete node
	~Node() {
		delete this->location;
	}
};

// Road network graph
class Graph {
public:
	int edgeNumber;										// Number of edges in the graph
	vector<Edge*> edgeList;								// Edges in the graph
	int nodeNumber;										// Number of nodes in the graph
	vector<Node*> nodeList;								// Nodes in the graph
	
	Graph(FileReader* frNode, FileReader* frEdge, FileReader* frGeo) {
		this->nodeNumber = 0;
		int tmpId = 0;
		while ((tmpId = frNode->nextInt()) != EOF) {
			++this->nodeNumber;
			nodeList.push_back(new Node(tmpId, frNode->nextDouble(), frNode->nextDouble()));
		}
		
		this->edgeNumber = 0;
		while ((tmpId = frEdge->nextInt()) != EOF) {
			++edgeNumber;
			
			Node* start = getNode(frEdge->nextInt());
			Node* end = getNode(frEdge->nextInt());
			Edge* edge = new Edge(tmpId, start, end);
			
			edgeList.push_back(edge);
			start->appendOutEdge(edge);
			
			frEdge->nextInt();
		}
		
		for (int i = 0; i < edgeNumber; ++i) {
			tmpId = frGeo->nextInt();
			Edge* edge = getEdge(tmpId);
			frGeo->nextChar();
			frGeo->nextChar();
			while (frGeo->nextChar() != '^') {}
			frGeo->nextInt();
			while (frGeo->nextChar() == '^') {
				double x = frGeo->nextDouble();
				frGeo->nextChar();
				double y = frGeo->nextDouble();
				edge->appendGeometry(new EcldPoint(x, y));
			}
			edge->setLength();
		}
	}
	
	// delete graph
	~Graph() {
		for (int i = 0; i < edgeNumber; ++i) {
			delete edgeList[i];
		}
		for (int i = 0; i < nodeNumber; ++i) {
			delete nodeList[i];
		}
	}
	
	// Get node by node id
	Node* getNode(int id) {
		if (nodeList[id]->id == id) {
			return nodeList[id];
		} else {
			for (int i = 0; i < nodeNumber; ++i) {
				if (nodeList[i]->id == id) {
					return nodeList[i];
				}
			}
		}
		return NULL;
	}
	
	// Get edge by edge id
	Edge* getEdge(int id) {
		if (edgeList[id]->id == id) {
			return edgeList[id];
		} else {
			for (int i = 0; i < edgeNumber; ++i) {
				if (edgeList[i]->id == id) {
					return edgeList[i];
				}
			}
		}
		return NULL;
	}
	
	// display the graph
	void display() {
		cout << "NODES" << endl;
		for (int i = 0; i < nodeNumber; ++i) {
			nodeList[i]->display();
			cout << endl;
		}
		cout << endl << endl;
		
		cout << "EDGES" << endl;
		for (int i = 0; i < edgeNumber; ++i) {
			edgeList[i]->display();
			cout << "Endpoints:" << edgeList[i]->startNode->id << " " << edgeList[i]->endNode->id << endl << endl;
		}
		cout << endl;
	}
	
	// Calculate shortest path table and store (using plain SPFA)
	void calSPTable(FileWriter* fw) {
		double* shortLen = new double[this->nodeNumber];
		int* pre = new int[this->nodeNumber];
		bool* visit = new bool[this->nodeNumber];
		queue<Node*> sequence;
		
		for (int s = 0; s < this->nodeNumber; ++s) {
			
			for (int j = 0; j < nodeNumber; ++j) {			// initialize single source shortest path
				shortLen[j] = Config::INFINITE_DISTANCE;
			}
			for (int j = 0; j < nodeNumber; ++j) {			// initialize visit table as all false
				visit[j] = false;
			}
			
			Node* startNode = this->getNode(s);
			shortLen[startNode->id] = 0;
			pre[startNode->id] = Config::NOT_CONNECTED_IN_ROAD_NETWORK;
			visit[startNode->id] = true;
			
			sequence.push(startNode);
			
			while (!sequence.empty()) {
				Node* tmp = sequence.front();				// check out current edge
				for (int i = 0; i < tmp->edgeNumber; ++i) {
					Edge* currentEdge = tmp->outEdges[i];
					if (shortLen[currentEdge->endNode->id] > shortLen[tmp->id] + currentEdge->len) {
						shortLen[currentEdge->endNode->id] = shortLen[tmp->id] + currentEdge->len;
						pre[currentEdge->endNode->id] = currentEdge->id;
						if (!visit[currentEdge->endNode->id]) {
							visit[currentEdge->endNode->id] = true;
							sequence.push(currentEdge->endNode);
							
						}
					}
				}
				
				visit[tmp->id] = false;						// get out of queue
				sequence.pop();
			}
			
			for (int t = 0; t < nodeNumber; ++t) {			// output the single source SP table
				if (shortLen[t] == Config::INFINITE_DISTANCE) {
					fw->writeInt(Config::NOT_CONNECTED_IN_ROAD_NETWORK);
				} else {
					fw->writeInt(pre[t]);
				}
				if (!fw->isBinary()) {
					fw->writeChar(' ');
				}
			}
			if (!fw->isBinary()) {
				fw->writeChar('\n');
			}
		}
		
		delete[] shortLen;									// release resource
		delete[] pre;
		delete[] visit;
	}
};

// Shorest Path table, support modification
// Use Singleton to avoid memory waste
class SPTable {
private:
	int nodeSize = 0;
	int ** table;
	static SPTable* instance;
	static vector<int>* pathBuffer;
	
	void checkBoudary(Graph* graph, int id) {
		if (id < 0 || id >= graph->edgeNumber) {
			throw "boundary exceed";
		}
	}
	
	SPTable() {
		// SP table not defined
		if (Config::SP_TABLE == NULL) {
			throw "SP Table path not assigned";
		}
		
		// get node number from SP table file size
		struct stat f_stat;
		if (stat(Config::SP_TABLE, &f_stat) == Config::FILE_SIZE_EMPTY) {
			throw "SP Table file error";
		}
		nodeSize = sqrt(f_stat.st_size / 4);
		table = new int*[nodeSize];
		for (int i = 0; i < nodeSize; ++i) {
			table[i] = new int[nodeSize];
		}
		
		// load SP table
		FileReader* fr = new FileReader(Config::SP_TABLE, true);
		for (int i = 0; i < nodeSize; ++i) {
			for (int j = 0; j < nodeSize; ++j) {
				table[i][j] = fr->nextInt();
			}
		}
		delete fr;
		
	}
	
public:
	static SPTable* getInstance() {
		if (instance == NULL) {
			instance = new SPTable();
		}
		return instance;
	}
	
	// get the previous edge of the SP path of two edges
	static int getPre(Graph* graph, int pre, int scc) {
		getInstance()->checkBoudary(graph, pre);
		getInstance()->checkBoudary(graph, scc);
		int startNodeId = graph->getEdge(pre)->endNode->id;
		int endNodeId = graph->getEdge(scc)->startNode->id;
		return getInstance()->table[startNodeId][endNodeId];
	}
	
	// get the shortest path between two edges (endpoints not included)
	static vector<int>* getPath(Graph* graph, int pre, int scc) {
		getInstance()->checkBoudary(graph, pre);
		getInstance()->checkBoudary(graph, scc);
		SPTable::pathBuffer->clear();
		
		int startNodeId = graph->getEdge(pre)->endNode->id;
		int endNodeId = graph->getEdge(scc)->startNode->id;
		
		while (startNodeId != endNodeId && getInstance()->table[startNodeId][endNodeId] != Config::NOT_CONNECTED_IN_ROAD_NETWORK) {
			pathBuffer->push_back(getInstance()->table[startNodeId][endNodeId]);
			endNodeId = graph->getEdge(getInstance()->table[startNodeId][endNodeId])->startNode->id;
		}

		reverse(SPTable::pathBuffer->begin(), SPTable::pathBuffer->end());
		return SPTable::pathBuffer;
	}
	
	~SPTable() {
		for (int i = 0; i < nodeSize; ++i) {
			delete[] table[i];
		}
	}
	
	static void free() {
		delete pathBuffer;
		if (instance != NULL) {
			delete instance;
		}
	}
};

SPTable* SPTable::instance = NULL;
vector<int>* SPTable::pathBuffer = new vector<int>();

#endif
