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
#include <queue>
#include "trajectory.h"
#include "config.h"
#include "counter_tool.h"

class ACNode {
public:
	int id;
	int value;						// edge id
	int father;						// father in the higher level
	int leftSon;					// left son in Trie
	int rightBrother;				// right brother in the same level
	int fail;						// fail pointer in matching
	int frequency;					// accur times in training dataset
	// for query
	int rootAncestor;				// the ancestor of the node which is the son of root
	
	ACNode(int id, int father, int value) {
		this->id = id;
		this->value = value;
		this->leftSon = Config::NULL_POINTER;
		this->rightBrother = Config::NULL_POINTER;
		this->father = father;
		this->frequency = 1;
		this->fail = 0;
		this->rootAncestor = Config::NULL_POINTER;
	}
	
	void display() {
		cout << id << " value: " << value
		<< "\t father: " << father << "\t leftSon: " << leftSon << "\t rightBrother: " << rightBrother
		<< "\t frequency: " << frequency << "\t fail: " << fail << endl;
	}
};

class ACAutomaton {
private:
	int getRootAncestor(int root, ACNode* node) {
		if (node->rootAncestor != Config::NULL_POINTER) return node->rootAncestor;
		if (node->id == root) return Config::NULL_POINTER;
		ACNode* father = getNode(node->father);
		if (father->father == root) {
			node->rootAncestor = father->id;
		}
		node->rootAncestor = getRootAncestor(root, father);
		return node->rootAncestor;
	}
	
public:
	int trieSize;
	bool rootAncestorInit = false;
	
	vector<ACNode*>* trie = new vector<ACNode*>();

	ACAutomaton(Graph* graph, vector<RoadNetTrajectory*>* spCompressedTrSet, int suffixMaxLength) {
		// Trajectory set must not be empty
		if (spCompressedTrSet->size() == 0) {
			throw "Trajetory set empty exception";
		}
		
		// Trie root
		trie->push_back(new ACNode(0, -1, Config::NULL_POINTER));
		trie->at(0)->frequency = 0;
		trieSize = 1;
		
		cout << "trajectory set" << endl;
		// each trajectory
		int trNumber = (int)spCompressedTrSet->size();
		for (int i = 0; i < trNumber; ++i) {
			if (i % 100 == 0) cout << "tra " << i << endl;
			
			RoadNetTrajectory* trajectory = spCompressedTrSet->at(i);
			int spLength = (int)trajectory->spatialNumber;
			
			if (spLength == 0) {
				throw "trajectory spatial component empty exception";
			}
			
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
		bool* inserted = new bool[graph->edgeNumber];
		for (int i = 0; i < graph->edgeNumber; ++i) {
			inserted[i] = false;
		}
		
		int pt = getNode(0)->leftSon, last = 0;
		while (pt != Config::NULL_POINTER) {
			inserted[getNode(pt)->value] = true;
			last = pt;
			pt = getNode(pt)->rightBrother;
		}
		for (int i = 0; i < graph->edgeNumber; ++i) {
			if (!inserted[i]) {
				getNode(last)->rightBrother = trieSize;
				trie->push_back(new ACNode(trieSize, 0, i));
				getNode(trieSize)->frequency = 0;
				last = trieSize;
				++trieSize;
			}
		}
		
		delete[] inserted;
		
		// add fail pointer
		cout << "fail pointer" << endl;
		cout << "total trie size:" << trieSize << endl;
		int count = 0;
		
		queue<ACNode*> sequence;
		pt = getNode(0)->leftSon;
		while (pt != Config::NULL_POINTER) {
			sequence.push(getNode(pt));
			pt = getNode(pt)->rightBrother;
		}
		while (!sequence.empty()) {
			ACNode* tmp = sequence.front();
			if (tmp->father != 0) {
				int pt = getNode(tmp->father)->fail;
				int fail;
				while ((fail = containSon(pt, tmp->value)) == Config::NULL_POINTER) {
					pt = getNode(pt)->fail;
				}
				tmp->fail = fail;
			}
			int pt = tmp->leftSon;
			while (pt != Config::NULL_POINTER) {
				sequence.push(getNode(pt));
				pt = getNode(pt)->rightBrother;
			}
			sequence.pop();
			if (count % 10000 == 0) cout << count << '\t';
			++count;
		}
		cout << endl;
	}
	
	ACAutomaton(FileReader* fr) {
		trieSize = fr->nextInt();
		for (int i = 0; i < trieSize; ++i) {
			ACNode* node = new ACNode(fr->nextInt(), fr->nextInt(), fr->nextInt());
			node->leftSon = fr->nextInt();
			node->rightBrother = fr->nextInt();
			node->frequency = fr->nextInt();
			node->fail = fr->nextInt();
			trie->push_back(node);
		}
	}
	
	void getFrequency() {
		for (int i = 0; i < this->trie->size(); ++i) {
			ACNode* node = this->trie->at(i);
			if (node->id) {
				int len = 1;
				int freq = node->frequency;
				while (node->father != 0) {
					++len;
					node = getNode(node->father);
				}
				CounterTool::getInstance()->fstFrequency[len] += freq;
			}
		}
	}
	
	void getRootAncestor() {
		for (int i = 0; i < trieSize; ++i) {
			getRootAncestor(0, trie->at(i));
		}
		this->rootAncestorInit = true;
	}
	
	int containSon(int id, int value) {
		int pt = getNode(id)->leftSon;
		while (pt != Config::NULL_POINTER) {
			if (getNode(pt)->value == value) return getNode(pt)->id;
			pt = getNode(pt)->rightBrother;
		}
		return Config::NULL_POINTER;
	}
	
	void store(FileWriter* fw) {
		fw->writeInt(trieSize);
		if (!fw->isBinary()) {
			fw->writeChar('\n');
		}
		for (int i = 0; i < trieSize; ++i) {
			if (fw->isBinary()) {
				fw->writeInt(trie->at(i)->id);
				fw->writeInt(trie->at(i)->father);
				fw->writeInt(trie->at(i)->value);
				fw->writeInt(trie->at(i)->leftSon);
				fw->writeInt(trie->at(i)->rightBrother);
				fw->writeInt(trie->at(i)->frequency);
				fw->writeInt(trie->at(i)->fail);
			} else {
				fw->writeInt(trie->at(i)->id);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->father);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->value);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->leftSon);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->rightBrother);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->frequency);
				fw->writeChar(' ');
				fw->writeInt(trie->at(i)->fail);
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
