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
#include "counter_tool.h"
#include "utility.h"
#include <vector>
#include <algorithm>

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
			int len = 1;
			while (ac->getNode(pt)->father != 0) {
				pt = ac->getNode(pt)->father;
				matchSet.pop_back();
				++len;
			}
			CounterTool::getInstance()->lenFrequency[len]++;	// count frequency of each length FST used
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
	
	// Complement spatial component from broken edge sequence
	static vector<int>* SPComplement(Graph* graph, vector<int>* source) {
		if (source == NULL || source->size() < 1) {
			throw "Spatial component empty";
		}
		vector<int>* result = new vector<int>();
		
		for (int i = (int)source->size() - 1; i > 0; --i) {
			int edge = source->at(i);
			result->push_back(edge);
			while (SPTable::getPre(graph, source->at(i - 1), edge) != Config::NULL_POINTER) {
				result->push_back(SPTable::getPre(graph, source->at(i - 1), edge));
				edge = SPTable::getPre(graph, source->at(i - 1), edge);
			}
		}
		result->push_back(source->at(0));
		reverse(result->begin(), result->end());
		
		return result;
	}
	
	// Decompress from binary code
	static vector<int>* FSTComplement(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, Binary* binary) {
		if (binary == NULL) {
			throw "Binary NULL";
		}
		vector<int>* result = new vector<int>();
		HuffmanNode* node = huffman->getNode(huffman->root);
		for (int i = 0; i < binary->number; ++i) {
			if (node->leftSon == Config::NULL_POINTER && node->rightSon == Config::NULL_POINTER) {
				if (binary->binary->at(i) == false) {
					node = huffman->getNode(node->leftSon);
				} else {
					node = huffman->getNode(node->rightSon);
				}
				result->push_back(((HuffmanLeafNode*)node)->trieId);
				node = huffman->getNode(huffman->root);
			}
		}
		return result;
	}
	
	
	// ========= BTC ==========
	static vector<TemporalPair*>* kernelBTC(vector<TemporalPair*>* temporal, vector<pair<int, int>*>* toleranceD, vector<pair<int, int>*>* toleranceT) {
		vector<TemporalPair*>* result = new vector<TemporalPair*>();
		result->push_back(temporal->at(0));
		if (temporal->size() > 1) {
			double lower = -1e100, higher = 1e100;
			int anchor = 0;
			for (int i = 1; i < temporal->size(); ++i) {
				double direction = angle(temporal->at(anchor)->t, temporal->at(anchor)->d, temporal->at(i)->t, temporal->at(i)->d);
				if (direction < lower || direction > higher) {
					result->push_back(temporal->at(i - 1));
					anchor = i - 1;
					lower = -1e100;
					higher = 1e100;
				}
				double tsndU = toleranceD->at(i)->first;
				double tsndB = toleranceD->at(i)->second;
				double nstdU = toleranceT->at(i)->first;
				double nstdB = toleranceT->at(i)->second;
				
				lower = max(lower, angle(temporal->at(anchor)->t, temporal->at(anchor)->d, temporal->at(i)->t, temporal->at(i)->d - tsndB));
				higher = min(higher, angle(temporal->at(anchor)->t, temporal->at(anchor)->d, temporal->at(i)->t, temporal->at(i)->d + tsndU));
				lower = max(lower, angle(temporal->at(anchor)->t, temporal->at(anchor)->d, temporal->at(i)->t + nstdU, temporal->at(i)->d));
				higher = min(higher, angle(temporal->at(anchor)->t, temporal->at(anchor)->d, temporal->at(i)->t - nstdB, temporal->at(i)->d));
			}
			result->push_back(temporal->at(temporal->size() - 1));
		}
		return result;
	}
	
	static vector<TemporalPair*>* basicBTC(vector<TemporalPair*>* temporal, double tsnd, double nstd) {
		vector<pair<int, int>*>* toleranceD = new vector<pair<int, int>*>();
		vector<pair<int, int>*>* toleranceT = new vector<pair<int, int>*>();
		
		for (int i = 0; i < temporal->size(); ++i) {
			toleranceD->push_back(new pair<int, int>(tsnd, tsnd));
			toleranceT->push_back(new pair<int, int>(nstd, nstd));
		}
		vector<TemporalPair*>* result = PRESS::kernelBTC(temporal, toleranceD, toleranceT);
		for (int i = 0; i < toleranceT->size(); ++i) {
			delete toleranceT->at(i);
			delete toleranceD->at(i);
		}
		delete toleranceT;
		delete toleranceD;
		return result;
	}
	
	static vector<TemporalPair*>* extendBTC(Graph* graph, RoadNetTrajectory* trajectory, double tsnd, double nstd) {
		vector<pair<int, int>*>* toleranceD = new vector<pair<int, int>*>();
		vector<pair<int, int>*>* toleranceT = new vector<pair<int, int>*>();
		double dist = 0;
		int flag = 0;
		for (int i = 0; i < trajectory->temporal->size(); ++i) {
			toleranceT->push_back(new pair<int, int>(nstd, nstd));
			while (flag < trajectory->spatial->size()-1 && dist + graph->getEdge(trajectory->spatial->at(flag))->len < trajectory->temporal->at(i)->d) {
				dist += graph->getEdge(trajectory->spatial->at(flag++))->len;
			}
			double tsndU = min(tsnd, dist + graph->getEdge(trajectory->spatial->at(flag))->len - trajectory->temporal->at(i)->d);
			double tsndB = min(tsnd, trajectory->temporal->at(i)->d - dist);
			toleranceD->push_back(new pair<int, int>(tsndU, tsndB));
		}
		vector<TemporalPair*>* result = PRESS::kernelBTC(trajectory->temporal, toleranceD, toleranceT);
		for (int i = 0; i < toleranceT->size(); ++i) {
			delete toleranceT->at(i);
			delete toleranceD->at(i);
		}
		delete toleranceD;
		delete toleranceT;
		return result;
	}
	
	// ========= PRESS ========
	// get compressed trajectory
	static PRESSCompressedTrajectory* compression(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, RoadNetTrajectory* trajectory, double tsnd, double nstd) {
		Binary* binary = PRESS::FSTCompression(ac, huffman, PRESS::SPCompression(graph, trajectory->spatial));
		vector<TemporalPair*>* temporal = PRESS::basicBTC(trajectory->temporal, tsnd, nstd);
		return new PRESSCompressedTrajectory(binary, temporal);
	}
	
	// de-compress PRESS compressed trajectory
	static RoadNetTrajectory* decompress(Graph* graph, ACAutomaton* ac, HuffmanTree* huffman, PRESSCompressedTrajectory* trajectory) {
		return new RoadNetTrajectory(
			SPComplement(graph,
				FSTComplement(graph, ac, huffman, trajectory->spatial)
			), trajectory->temporal
		);
	}
};

#endif
