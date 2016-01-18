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
	
    // Get road network trajectory from GPS trajectory and map match result
//	PreProcessor::getInstance()->generateRoadNetTrajectory(
//		graph,
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/input2",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/output2",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/spatial2.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/temporal2.txt"
//	);
	
//	PreProcessor::getInstance()->generateRoadNetTrajectory(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/input2_busy",
//		"/Users/songrenchu/Develop/test/free_busy/output2_busy",
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_busy.txt"
//	);
//
//	PreProcessor::getInstance()->generateRoadNetTrajectory(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/input2_free",
//		"/Users/songrenchu/Develop/test/free_busy/output2_free",
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_free.txt"
//	);


//	// Get SP compressed spatial component from training dataset
//	Experiment::getInstance()->trainingSPCompress(
//		graph,
//		"/Users/songrenchu/Develop/test/spatial1.txt",
//		"/Users/songrenchu/Develop/test/temporal1.txt",
//		"/Users/songrenchu/Develop/test/comp.txt"
//	);
	
//	Experiment::getInstance()->trainingSPCompress(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/spatial1_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal1_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/comp_1_busy.txt"
//	);
//
//	Experiment::getInstance()->trainingSPCompress(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/spatial1_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal1_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/comp_1_free.txt"
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

//	// Generate AC automaton and Huffman tree from SP compressed component
//	Experiment::getInstance()->generateACHuffman(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/comp_1_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal1_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/ac_1_busy.txt",
//		3,
//		"/Users/songrenchu/Develop/test/free_busy/huffman_1_busy.txt"
//	);
//	// Generate AC automaton and Huffman tree from SP compressed component
//	Experiment::getInstance()->generateACHuffman(
//		graph,
//		"/Users/songrenchu/Develop/test/free_busy/comp_1_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal1_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/ac_1_free.txt",
//		3,
//		"/Users/songrenchu/Develop/test/free_busy/huffman_1_free.txt"
//	);
	
//	// PRESS compression, get FST used frequency of each length
//	FileReader* acReader = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/ac.txt", true);
//	ACAutomaton* ac = new ACAutomaton(acReader);
//	delete acReader;
//	
//	FileReader* huffmanReader = new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/huffman.txt", true);
//	HuffmanTree* huffman = new HuffmanTree(huffmanReader);
//	delete huffmanReader;
//	
//	
//	Experiment::getInstance()->pressCompress(
//		graph,
//		ac,
//		huffman,
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/spatial2.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/RoadNetTrajectory/temporal2.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/CompressedTrajectory/hsc_compressed_2.txt",
//		"/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/CompressedTrajectory/btc_compressed_2.txt",
//		10,
//		10
//	);
//
//	int total = 0;
//	for (int i = 0; i < 100; ++i) {
//		total += CounterTool::getInstance()->lenFrequency[i];
//	}
//	for (int i = 0; i < 100; ++i) {
//		if (CounterTool::getInstance()->lenFrequency[i] > 0) {
//			cout << "Len " << i << ": " << CounterTool::getInstance()->lenFrequency[i] / (double)total << endl;
//		}
//	}
//
	
	
//	// AC automaton frequency count
//	ACAutomaton* ac = new ACAutomaton(new FileReader("/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/v2/AC_Huffman/ac.txt", true));
//	ac->getFrequency();
//	int total = 0;
//	for (int i = 0; i < 100; ++i) {
//		total += CounterTool::getInstance()->fstFrequency[i];
//	}
//	for (int i = 0; i < 100; ++i) {
//		if (CounterTool::getInstance()->fstFrequency[i] > 0) {
//			cout << "Len " << i << ": " << CounterTool::getInstance()->fstFrequency[i] / (double)total << endl;
//		}
//	}
//
//	
//	FileReader* acReader = new FileReader("/Users/songrenchu/Develop/test/free_busy/ac_1_busy.txt", true);
//	ACAutomaton* ac = new ACAutomaton(acReader);
//	delete acReader;
//
//	FileReader* huffmanReader = new FileReader("/Users/songrenchu/Develop/test/free_busy/huffman_1_busy.txt", true);
//	HuffmanTree* huffman = new HuffmanTree(huffmanReader);
//	delete huffmanReader;
//
//
//	Experiment::getInstance()->pressCompress(
//		graph,
//		ac,
//		huffman,
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_busy.txt",
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_busy_compressed.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_busy_compressed.txt",
//		10,
//		10
//	);
//	Experiment::getInstance()->pressCompress(
//		graph,
//		ac,
//		huffman,
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_free.txt",
//		"/Users/songrenchu/Develop/test/free_busy/spatial2_free_compressed.txt",
//		"/Users/songrenchu/Develop/test/free_busy/temporal2_free_compressed.txt",
//		10,
//		10
//	);
	
	
	SPTable::free();
	return 0;

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
}

