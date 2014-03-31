//
//  main.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 4/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include <iostream>
#include "file_processor.h"
#include "trajectory.h"
#include "road_network.h"
#include "config.h"
#include "utility.h"
#include "press.h"
#include "ac_automaton.h"
#include "huffman.h"
#include "timer.h"
#include "pre_processing.h"
#include "experiment.h"
#include <vector>
#include "counter_tool.h"

using namespace std;

// Initialize the system
void systemInitialize() {
	Config::ROAD_NETWORK_NODE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_Nodes.txt";
	Config::ROAD_NETWORK_EDGE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_Edges.txt";
	Config::ROAD_NETWORK_GEOMETRY = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_EdgeGeometry.txt";
	Config::SP_TABLE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/ShortestPath/SPTable.txt";
}

int main(int argc, const char * argv[]) {
	
	// Initialize the system with hard coded urls;
	systemInitialize();
	
	// Road network
	FileReader* nodeReader = new FileReader(Config::ROAD_NETWORK_NODE, false);
	FileReader* edgeReader = new FileReader(Config::ROAD_NETWORK_EDGE, false);
	FileReader* geoReader = new FileReader(Config::ROAD_NETWORK_GEOMETRY, false);
	
	Graph* graph = new Graph(nodeReader, edgeReader, geoReader);
	
	delete nodeReader;
	delete edgeReader;
	delete geoReader;
	
	SPTable::getInstance();
	
//  // Get road network trajectory from GPS trajectory and map match result
//	PreProcessor::getInstance()->generateRoadNetTrajectory(
//		graph,
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/input2",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/output2",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/spatial2.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/temporal2.txt"
//	);


//	// Get SP compressed spatial component from training dataset
//	Experiment::getInstance()->trainingSPCompress(
//		graph,
//		"/Users/songrenchu/Develop/test/spatial1.txt",
//		"/Users/songrenchu/Develop/test/temporal1.txt",
//		"/Users/songrenchu/Develop/test/comp.txt"
//	);
	
//	// Generate AC automaton and Huffman tree from SP compressed component
//	Experiment::getInstance()->generateACHuffman(
//		graph,
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/CompressedTrajectory/sp_compressed_1.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/temporal1.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/ac.txt",
//		3,
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/huffman.txt"
//	);
	
	// PRESS compression, get FST used frequency of each length
	FileReader* acReader = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/ac.txt", true);
	ACAutomaton* ac = new ACAutomaton(acReader);
	delete acReader;
	
	FileReader* huffmanReader = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/huffman.txt", true);
	HuffmanTree* huffman = new HuffmanTree(huffmanReader);
	delete huffmanReader;
	
	
	Experiment::getInstance()->pressCompress(
		graph,
		ac,
		huffman,
		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/spatial2.txt",
		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/temporal2.txt",
		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/CompressedTrajectory/hsc_compressed_2.txt",
		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/CompressedTrajectory/btc_compressed_2.txt",
		10,
		10
	);

	int total = 0;
	for (int i = 0; i < 100; ++i) {
		total += CounterTool::getInstance()->lenFrequency[i];
	}
	for (int i = 0; i < 100; ++i) {
		if (CounterTool::getInstance()->lenFrequency[i] > 0) {
			cout << "Len " << i << ": " << CounterTool::getInstance()->lenFrequency[i] / (double)total << endl;
		}
	}

	SPTable::free();
	return 0;
//
////	vector<char*>* list = FileTool::getInstance()->getFileNameSet("/Users/songrenchu/Develop/test/");
////	for (int i = 0; i < list->size(); ++i) {
////		cout << list->at(i) << endl;
////	}
////	
//////	FileReader* fr = new FileReader("/Users/songrenchu/Develop/test/test.txt", false);
//////	cout << fr->nextString() << endl << fr->nextString() << endl << fr->nextString() << endl << fr->nextString() << endl;
//////	if (fr->nextString() == NULL) cout << "dfdf" << endl;
////	return 0;
//	
////	vector<bool>* binary = new vector<bool>();
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(false);
////	binary->push_back(false);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(true);
////	binary->push_back(true);
////	
////	
//////	Binary b(binary);
//////	b.store(new FileWriter("/Users/songrenchu/Develop/test/binary.txt", true));
////
////	
////	Binary b(new FileReader("/Users/songrenchu/Develop/test/binary.txt", true));
////	b.display();
////	
////	return 0;
//	
//////	Timer* timer = new Timer();
//////	timer->resetTimer();
//////	for (int i = 0; i < 1000000000; ++i);
//////	cout << timer->getSystemClockDuration() << endl;
//////	
//////	return 0;
//	
//
//
//////
//////	g->edgeNumber = 10;
//////	
//////	vector<RoadNetTrajectory*>* traSet = new vector<RoadNetTrajectory*>();
//////	
//////	vector<int>* spatial = new vector<int>();
//////	spatial->push_back(1);
//////	spatial->push_back(5);
//////	spatial->push_back(8);
//////	spatial->push_back(6);
//////	spatial->push_back(3);
//////	vector<TemporalPair>* temporal = new vector<TemporalPair>();
//////	RoadNetTrajectory* tra1 = new RoadNetTrajectory(spatial, temporal);
//////	traSet->push_back(tra1);
//////	
//////	vector<int>* spatial2 = new vector<int>();
//////	spatial2->push_back(1);
//////	spatial2->push_back(5);
//////	spatial2->push_back(2);
//////	spatial2->push_back(1);
//////	spatial2->push_back(4);
//////	spatial2->push_back(8);
//////	RoadNetTrajectory* tra2 = new RoadNetTrajectory(spatial2, temporal);
//////	traSet->push_back(tra2);
//////	
//////	vector<int>* spatial3 = new vector<int>();
//////	spatial3->push_back(2);
//////	spatial3->push_back(1);
//////	spatial3->push_back(4);
//////	spatial3->push_back(6);
//////	RoadNetTrajectory* tra3 = new RoadNetTrajectory(spatial3, temporal);
//////	traSet->push_back(tra3);
//////	
//////	ACAutomaton* ac = new ACAutomaton(g, traSet, 3);
//////	
//////	HuffmanTree* huffman = new HuffmanTree(ac);
//////
////	HuffmanTree* huffman = new HuffmanTree(new FileReader("/Users/songrenchu/Develop/test/huffman.txt", true));
//////	huffman->display();
//////	
//////	huffman->displayCode();
////	
//////	huffman->store(new FileWriter("/Users/songrenchu/Develop/test/huffman.txt", true));
////	
////	
////	//ac->display();
////	//ac->store(new FileWriter("/Users/songrenchu/Develop/test/ac.txt", true));
//////
////	ACAutomaton* ac = new ACAutomaton(new FileReader("/Users/songrenchu/Develop/test/ac.txt", true));
//////	ac->display();
////	
////	
////	vector<int>* spatial = new vector<int>();
////	spatial->push_back(1);
////	spatial->push_back(4);
////	spatial->push_back(7);
////	spatial->push_back(5);
////	spatial->push_back(8);
////	spatial->push_back(6);
////	spatial->push_back(3);
////	spatial->push_back(1);
////	spatial->push_back(5);
////	spatial->push_back(2);
////	spatial->push_back(0);
////	
//////	vector<TemporalPair>* temporal = new vector<TemporalPair>();
//////	RoadNetTrajectory* tra1 = new RoadNetTrajectory(spatial, temporal);
//////	traSet->push_back(tra1);
////	
////	PRESS::FSTCompression(ac, huffman, spatial)->display();
////	
////	return 0;
////	
//	
//	
//	//	vector<int>* path = SPTable::getPath(g, 19304, 19393);
//	//	for (int i = 0; i < path->size(); ++i) {
//	//		cout << path->at(i) << " ";
//	//	}
//	//	cout << endl;
//	//
//	//	path = SPTable::getPath(g, 19393, 19098);
//	//	for (int i = 0; i < path->size(); ++i) {
//	//		cout << path->at(i) << " ";
//	//	}
//	//	cout << endl;
//	//
//	//	path = SPTable::getPath(g, 19304, 19098);
//	//	for (int i = 0; i < path->size(); ++i) {
//	//		cout << path->at(i) << " ";
//	//	}
//	//	cout << endl;
//	//
//	//	return 0;
//	
//	//	g->getEdge(19393)->endNode->display();
//	//	g->getEdge(19097)->startNode->display();
//	//	
//	//	return 0;
//	
//	
//
//	FileReader* spatial = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/v2_SPCompressedSpatial_2.txt", true);
//	FileReader* temporal = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v1/Temporal_2.txt", true);
//	FileWriter* newSpatial = new FileWriter("/Users/songrenchu/Develop/test/v2spatial.txt", true);
//
//	
//	int trNumber = spatial->nextInt();
//	
//	newSpatial->writeInt(trNumber);
//	
//	temporal->nextInt();
//
//	for (int i = 0; i < trNumber; ++i) {
//		if (i % 1000 == 0) {
//			cout << i << endl;
//		}
//		
//		RoadNetTrajectory* trajectory = new RoadNetTrajectory(spatial, temporal);
//		vector<int>* spComponent = PRESS::SPComplement(g, trajectory->spatial);
//		
//		newSpatial->writeInt((int)spComponent->size());
//		
//		for (int j = 0; j < spComponent->size(); ++j) {
//			newSpatial->writeInt(spComponent->at(j));
//		}
//	}
//	
//	
//	//
//	//		vector<int>* extend = new vector<int>();
//	//
//	//		for (int j = 0; j < spComponent->size() - 1; ++j) {
//	//			extend->push_back(spComponent->at(j));
//	//			vector<int>* path = SPTable::getPath(g, spComponent->at(j), spComponent->at(j+1));
//	//			for (int k = 0; k < path->size(); ++k) {
//	//				extend->push_back(path->at(k));
//	//			}
//	//		}
//	//		extend->push_back(spComponent->at(spComponent->size() - 1));
//	//
//	//		newSpatial->writeInt((int)extend->size());
//	//
//	//		for (int j = 0; j < extend->size(); ++j) {
//	//			newSpatial->writeInt(extend->at(j));
//	//		}
//	
//	//		extend->clear();
//	
//	
//
////	vector<int>* src = SPTable::getInstance()->getPath(g, 1, 100);
////	
////	for (int i = 0; i < src->size(); ++i) {
////		cout << (*src)[i] << "\t";
////	}
////	cout << endl;
//	
//    /*FileWriter* fw = new FileWriter("/Users/songrenchu/Develop/test/test.txt", false);
//	fw->writeInt(123);
//	fw->writeChar(' ');
//	fw->writeDouble(21312123.53453434432);
//	fw->writeChar('A');
//	fw->writeChar(' ');
//	fw->writeShort((short)101);
//	fw->~FileWriter();
//	
//	FileReader* fr = new FileReader("/Users/songrenchu/Develop/test/test.txt", false);
//	cout << fr->nextInt() << endl;
//	cout << fr->nextDouble() << endl;
//	cout << fr->nextChar() << endl;
//	cout << fr->nextShort() << endl;
//	
//	GPSTrajectory* gps = new GPSTrajectory(new FileReader("/Users/songrenchu/Develop/test/test.txt", false));
//	
//	gps->store(new FileWriter("/Users/songrenchu/Develop/test/test2.txt", true));
//	*/
//	
//	
//	
////	Graph* g = new Graph(
////						 new FileReader(Config::ROAD_NETWORK_NODE, false),
////						 new FileReader(Config::ROAD_NETWORK_EDGE, false),
////						 new FileReader(Config::ROAD_NETWORK_GEOMETRY, false)
////	);
////	
////	
////	g->display();
////	
//	//g->calSPTable(new FileWriter("/Users/songrenchu/Develop/test/SP.txt", true));
//	
    
}

