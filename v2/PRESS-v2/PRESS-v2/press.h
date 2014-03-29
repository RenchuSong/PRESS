//
//  press.h
//  PRESS-v2
//
//  Created by Renchu Song on 19/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_press_h
#define PRESS_v2_press_h

#include "road_network.h"
#include "file_processor.h"
#include "ac_automaton.h"
#include "huffman.h"
#include <vector>

using namespace std;

// PRESS algorithm factory
class PRESS {
public:
	// ========= HSC ==========
	// SP compress, get a compressed spatial component from an original spatial component
	static vector<int>* SPCompression(Graph* graph, vector<int>* source) {
		if (source == NULL || source->size() < 1) {
			throw "Spatial component empty";
		}
		vector<int>* result = new vector<int>();
		result->push_back(source->at(0));
		int preEdge = source->at(0);
		int pointNumber = (int)source->size();
		for (int i = 2; i < pointNumber; ++i) {
			if (SPTable::getPre(graph, preEdge, source->at(i)) != source->at(i - 1)) {
				result->push_back(source->at(i - 1));
				preEdge = source->at(i - 1);
			}
		}
		if (pointNumber > 1) {
			result->push_back(source->at(pointNumber - 1));
		}
		return result;
	}
	
	// FST compress, get a compressed spatial bit stream from an original spatial component
	static Binary* FSTCompression(ACAutomaton* ac, HuffmanTree* huffman, vector<int>* source) {
		if (source == NULL || source->size() < 1) {
			throw "Spatial component empty";
		}
		
		// get AC node set
		vector<int> matchSet;
		
		int pt = 0;
		for (int i = 0; i < source->size(); ++i) {
			while (ac->containSon(pt, source->at(i)) == Config::NULL_POINTER) {
				pt = ac->getNode(pt)->fail;
			}
			pt = ac->containSon(pt, source->at(i));
			matchSet.push_back(pt);
		}
		
		vector<int> compressedSet;
		while (!matchSet.empty()) {
			int pt = matchSet[matchSet.size() - 1];
			compressedSet.push_back(pt);
			matchSet.pop_back();
			while (ac->getNode(pt)->father != 0) {
				pt = ac->getNode(pt)->father;
				matchSet.pop_back();
			}
		}
		
		matchSet.clear();
		reverse(compressedSet.begin(), compressedSet.end());

		// combine binary code
		vector<bool>* code = new vector<bool>();
		for (int i = 0; i < compressedSet.size(); ++i) {
			HuffmanCode* item = huffman->trieHuffmanCode[compressedSet[i]];
			copy(item->code.begin(), item->code.end(), back_inserter(*code));
		}
		
		Binary* binary = new Binary(code);
		return binary;
	}
	
	// ========= BTC ==========
	static vector<TemporalPair>* basicBTC(RoadNetTrajectory* trajectory, double tsnd, double nstd) {
		/** TODO to be impelmented */
		return trajectory->temporal;
	}
	
	// ========= PRESS ========
	// get compressed trajectory
	PRESSCompressedTrajectory* compression(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, RoadNetTrajectory* trajectory, double tsnd, double nstd) {
		Binary* binary = PRESS::FSTCompression(ac, huffman, PRESS::SPCompression(graph, trajectory->spatial));
		vector<TemporalPair>* temporal = PRESS::basicBTC(trajectory, tsnd, nstd);
		return new PRESSCompressedTrajectory(binary, temporal);
	}
};

#endif
