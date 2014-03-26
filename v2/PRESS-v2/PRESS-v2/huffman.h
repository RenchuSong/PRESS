//
//  huffman.h
//  PRESS-v2
//
//  Created by Renchu Song on 24/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_huffman_h
#define PRESS_v2_huffman_h

#include <vector>
#include <algorithm>
#include "ac_automaton.h"
#include "config.h"

class HuffmanNode {
public:
	int id;
	int weight;
	int father;
	int leftSon;
	int rightSon;
	
	HuffmanNode(int id, int weight, int leftSon, int rightSon) {
		this->id = id;
		this->weight = weight;
		this->leftSon = leftSon;
		this->rightSon = rightSon;
	}
	
	void display() {
		cout << id << " weight: " << weight
		<< "\t father: " << father << "\t leftSon: " << leftSon << "\t rightSon: " << rightSon << endl;
	}
};

int cmp(const ACNode* pre, const ACNode* scc) {
	if (pre->frequency < scc->frequency) {
		return 1;
	} else {
		return 0;
	}
}

class HuffmanTree {
private:
	void display(HuffmanNode* node) {
		node->display();
		if (node->leftSon != Config::NULL_POINTER) {
			display(getNode(node->leftSon));
		}
		if (node->rightSon != Config::NULL_POINTER) {
			display(getNode(node->rightSon));
		}
	}
	
public:
	int huffmanSize;
	vector<HuffmanNode*>* huffman = new vector<HuffmanNode*>();
	int* trieToHuffman = NULL;			// map trie node id to huffman tree node id
	int root;
	
	HuffmanTree(ACAutomaton* acAutomaton) {
		if (acAutomaton == NULL) {
			throw "AC Automaton NULL exception";
		}
		trieToHuffman = new int[acAutomaton->trieSize];
		ACNode** acNodeList = new ACNode*[acAutomaton->trieSize];
		for (int i = 0; i < acAutomaton->trieSize; ++i) {
			acNodeList[i] = acAutomaton->trie->at(i);
		}
		
		sort(acNodeList, acNodeList + acAutomaton->trieSize, cmp);
		
		vector<int> hfmBranchNode;
		
		huffmanSize = 0;
		int ptFirst = 0, ptSecond = 0;
		
		for (int i = 1; i < acAutomaton->trieSize; ++i) {
			int minFrequency = Config::HUGE_NUMBER;
			int operation;
			if (ptFirst < acAutomaton->trieSize - 1 && minFrequency > acNodeList[ptFirst]->frequency + acNodeList[ptFirst + 1]->frequency) {
				minFrequency = acNodeList[ptFirst]->frequency + acNodeList[ptFirst + 1]->frequency;
				operation = 1;
			}
			if (ptSecond < hfmBranchNode.size() - 1 && minFrequency > getNode(hfmBranchNode[ptSecond])->weight + getNode(hfmBranchNode[ptSecond + 1])->weight) {
				minFrequency = getNode(hfmBranchNode[ptSecond])->weight + getNode(hfmBranchNode[ptSecond + 1])->weight;
				operation = 2;
			}
			if (ptFirst < acAutomaton->trieSize && ptSecond < hfmBranchNode.size() && minFrequency > acNodeList[ptFirst]->frequency + getNode(hfmBranchNode[ptSecond])->weight) {
				minFrequency = acNodeList[ptFirst]->frequency + getNode(hfmBranchNode[ptSecond])->weight;
				operation = 3;
			}
			
			switch (operation) {
				case 1:
					huffman.
			}
		}
		
		hfmBranchNode.clear();
		delete[] acNodeList;
	}
	
	HuffmanNode* getNode(int id) {
		if (this->huffman->at(id)->id == id) {
			return this->huffman->at(id);
		}
		for (int i = 0; i < huffmanSize; ++i) {
			if (this->huffman->at(i)->id == id) {
				return this->huffman->at(i);
			}
		}
		return NULL;
	}
	
	void display() {
		display(getNode(root));
	}
	
	~HuffmanTree() {
		if (trieToHuffman != NULL){
			delete[] trieToHuffman;
		}
	}
};

class HaffmanCode {
public:
	int trieId;
	int byteNum;
	byte
};
#endif
