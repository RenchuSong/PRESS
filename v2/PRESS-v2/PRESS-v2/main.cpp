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
#include <vector>

using namespace std;

// Initialize the system
void systemInitialize() {
	Config::ROAD_NETWORK_NODE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_Nodes.txt";
	Config::ROAD_NETWORK_EDGE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_Edges.txt";
	Config::ROAD_NETWORK_GEOMETRY = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/WA_EdgeGeometry.txt";
	Config::SP_TABLE = "/Users/songrenchu/百度云同步盘/PRESS_SampleDataset/ShortestPath/SPTable.txt";
}

int main(int argc, const char * argv[])
{
	// Initialize the system with hard coded urls;
	systemInitialize();
	
	vector<int>* src = SPTable::getInstance()->getPath(1, 100);
	
	for (int i = 0; i < src->size(); ++i) {
		cout << (*src)[i] << "\t";
	}
	cout << endl;

	return 0;
	
	
	
	
	
	
	
    /*FileWriter* fw = new FileWriter("/Users/songrenchu/Develop/test/test.txt", false);
	fw->writeInt(123);
	fw->writeChar(' ');
	fw->writeDouble(21312123.53453434432);
	fw->writeChar('A');
	fw->writeChar(' ');
	fw->writeShort((short)101);
	fw->~FileWriter();
	
	FileReader* fr = new FileReader("/Users/songrenchu/Develop/test/test.txt", false);
	cout << fr->nextInt() << endl;
	cout << fr->nextDouble() << endl;
	cout << fr->nextChar() << endl;
	cout << fr->nextShort() << endl;
	
	GPSTrajectory* gps = new GPSTrajectory(new FileReader("/Users/songrenchu/Develop/test/test.txt", false));
	
	gps->store(new FileWriter("/Users/songrenchu/Develop/test/test2.txt", true));
	*/
	
	
	
//	Graph* g = new Graph(
//						 new FileReader(Config::ROAD_NETWORK_NODE, false),
//						 new FileReader(Config::ROAD_NETWORK_EDGE, false),
//						 new FileReader(Config::ROAD_NETWORK_GEOMETRY, false)
//	);
//	
//	
//	g->display();
//	
	//g->calSPTable(new FileWriter("/Users/songrenchu/Develop/test/SP.txt", true));
	
    
}

