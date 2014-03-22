//
//  ac_automaton.h
//  PRESS-v2
//
//  Created by Renchu Song on 20/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_ac_automaton_h
#define PRESS_v2_ac_automaton_h

#include <vector>
#include "trajectory.h"
#include "config.h"

class ACNode {
public:
	int id;
	int value;						// edge id
	int father;						// father in the higher level
	int leftSon;					// left son in Trie
	int rightBrother;				// right brother in the same level
	int fail;						// fail pointer in matching
	int frequency;					// accur times in training dataset
	
	ACNode(int id, int value, int father) {
		this->id = id;
		this->value = value;
		this->leftSon = Config::NULL_POINTER;
		this->rightBrother = Config::NULL_POINTER;
		this->father = father;
		this->frequency = 1;
	}
	
	void display() {
		cout << id << " value: " << value
		<< " leftSon: " << leftSon << " rightBrother: " << rightBrother << " father: " << father
		<< " frequency: " << frequency << endl;
	}
};

class ACAutomaton {
private:
	int containSon(int id, int value) {
		int pt = getNode(id)->leftSon;
		while (pt != Config::NULL_POINTER) {
			if (getNode(pt)->value == value) return getNode(pt)->id;
			pt = getNode(pt)->rightBrother;
		}
		return Config::NULL_POINTER;
	}
	
public:
	int trieSize;
	vector<ACNode*>* trie = new vector<ACNode*>();

	ACAutomaton(Graph* graph, vector<RoadNetTrajectory*>* spCompressedTrSet, int suffixMaxLength) {
		// Trie root
		trie->push_back(new ACNode(0, -1, Config::NULL_POINTER));
		trieSize = 1;
		
		// each trajectory
		int trNumber = (int)spCompressedTrSet->size();
		for (int i = 0; i < trNumber; ++i) {
			RoadNetTrajectory* trajectory = spCompressedTrSet->at(i);
			int spLength = (int)trajectory->spatialNumber;
			for (int start = 0; start < spLength; ++start) {
				int end = start + suffixMaxLength;
				if (end > spLength) {
					end = spLength;
				}
				int pt = 0;
				for (int loc = start; loc < end; ++loc) {
					if (getNode(pt)->leftSon == Config::NULL_POINTER) {
						getNode(pt)->leftSon = trieSize;
						trie->push_back(new ACNode(trieSize, pt, trajectory->spatial->at(loc)));
						pt = trieSize;
						++trieSize;
					} else {
						//int find = 0;
						int tmp = getNode(pt)->leftSon;
						int last;
						while (tmp != Config::NULL_POINTER && getNode(tmp)->value != trajectory->spatial->at(loc)) {
							last = tmp;
							tmp = getNode(tmp)->rightBrother;
						}
						if (tmp == Config::NULL_POINTER) {
							getNode(last)->rightBrother = trieSize;
							trie->push_back(new ACNode(trieSize, pt, trajectory->spatial->at(loc)));
							pt = trieSize;
							++trieSize;
						} else {
							pt = tmp;
							++getNode(pt)->frequency;
						}
					}
				}
				
				
			}
		}
		
		// add not shown edges
		
		// add fail pointer
		
	}
	
	ACAutomaton(FileReader* fr) {
		trieSize = fr->nextInt();
		for (int i = 0; i < trieSize; ++i) {
			ACNode* node = new ACNode(fr->nextInt(), fr->nextInt(), fr->nextInt());
			node->leftSon = fr->nextInt();
			node->rightBrother = fr->nextInt();
			node->frequency = fr->nextInt();
			trie->push_back(node);
		}
	}
	
	void store(FileWriter* fw) {
		fw->writeInt(trieSize);
		if (fw->isBinary()) {
			fw->writeChar('\n');
		}
		for (int i = 0; i < trieSize; ++i) {
			if (fw->isBinary()) {
				fw->writeInt(trie->at(i)->id);
				fw->writeInt(trie->at(i)->value);
				fw->writeInt(trie->at(i)->father);
				fw->writeInt(trie->at(i)->leftSon);
				fw->writeInt(trie->at(i)->rightBrother);
				fw->writeInt(trie->at(i)->frequency);
			} else {
				fw->writeInt(trie->at(i)->id);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->value);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->father);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->leftSon);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->rightBrother);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->frequency);
				fw->writeChar('\n');
			}
		}
	}
	
	ACNode* getNode(int id) {
		if (this->trie->at(id)->id == id) {
			return this->trie->at(id);
		}
		for (int i = 0; i < trieSize; ++i) {
			if (this->trie->at(i)->id == id) {
				return this->trie->at(i);
			}
		}
		return NULL;
	}
	
	void display() {
		for (int i = 0; i < trieSize; ++i) {
			trie->at(i)->display();
		}
	}
	
	~ACAutomaton() {
		for (int i = 0; i < trieSize; ++i) {
			delete trie->at(i);
		}
	}
};

#endif
