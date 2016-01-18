//
//  config.h
//  PRESS-v2
//
//  Created by Renchu Song on 18/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//
// In this class you can set and get global variables that can be used in all classes
// For example, file paths can be set during initialization of the program and used in classes
// Use singleton to guarantee there is only one instance of Config in this system

#ifndef PRESS_v2_config_h
#define PRESS_v2_config_h

using namespace std;

class Config {
public:
	
	// File paths
	static char* ROAD_NETWORK_NODE;			// road network node file
	static char* ROAD_NETWORK_EDGE;			// road network edge file
	static char* ROAD_NETWORK_GEOMETRY;		// road network geometry file
	
	static char* SP_TABLE;					// shortest path table path
	
	// Global variables
	static const int FILE_SIZE_EMPTY;		// file size error
	static const int NOT_CONNECTED_IN_ROAD_NETWORK;	// not connected
	static const double INFINITE_DISTANCE;	// from infinite distance
	static const double NULL_POINTER;		// null pointer expressed by integer
	static const int HUGE_NUMBER;			// huge number
	static const double EBSILON;			// small number
	
	
	
};

char* Config::ROAD_NETWORK_NODE = NULL;
char* Config::ROAD_NETWORK_EDGE = NULL;
char* Config::ROAD_NETWORK_GEOMETRY = NULL;
char* Config::SP_TABLE = NULL;

const int Config::FILE_SIZE_EMPTY = -1;
const int Config::NOT_CONNECTED_IN_ROAD_NETWORK = -1;
const double Config::INFINITE_DISTANCE = 1e100;
const double Config::NULL_POINTER = -1;
const int Config::HUGE_NUMBER = 1047483648;
const double Config::EBSILON = 1e-8;

#endif