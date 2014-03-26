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

// compare two ACNode
int cmp(const ACNode* pre, const ACNode* scc) {
	if (pre->frequency < scc->frequency) {
		return 1;
	} else {
		return 0;
	}
}

// Huffman Tree Structure Node
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
// Leaf Huffman Node has Trie tag
class HuffmanLeafNode: public HuffmanNode {
public:
	int trieId;
	
	HuffmanLeafNode(int id, int trieId, int weight, int leftSon, int rightSon): HuffmanNode(id, weight, leftSon, rightSon) {
		this->trieId = trieId;
	}
	
	void display() {
		cout << id << " weight: " << weight
		<< "\t father: " << father << "\t leftSon: " << leftSon << "\t rightSon: " << rightSon
		<< "\t trie: " << trieId << endl;
	}
};

// Huffman Code Binary Code;
class HuffmanCode {
public:
	int trieId;
	vector<bool> code;
};

// Huffman Tree Structure
class HuffmanTree {
private:
	// recursively display Huffman Tree
	void display(HuffmanNode* node) {
		if (node->leftSon == Config::NULL_POINTER && node->rightSon == Config::NULL_POINTER) {
			((HuffmanLeafNode*)node)->display();
		} else {
			node->display();
		}
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
	HuffmanCode** trieHuffmanCode = NULL;	// each trie node's huffman code
	int root;
	
	HuffmanTree(ACAutomaton* acAutomaton) {
		if (acAutomaton == NULL) {
			throw "AC Automaton NULL exception";
		}
		trieToHuffman = new int[acAutomaton->trieSize];					//
		trieHuffmanCode = new HuffmanCode*[acAutomaton->trieSize];		// the huffman code of each trie node
		ACNode** acNodeList = new ACNode*[acAutomaton->trieSize];
		for (int i = 0; i < acAutomaton->trieSize; ++i) {
			acNodeList[i] = acAutomaton->trie->at(i);
		}
		
		sort(acNodeList, acNodeList + acAutomaton->trieSize, cmp);
		
		vector<int> hfmBranchNode;
		
		int ptFirst = 0, ptSecond = 0;
		huffmanSize = 0;
		for (int i = 0; i < acAutomaton->trieSize; ++i) {
			int minFrequency = Config::HUGE_NUMBER;
			int operation = 0;
			if (ptFirst < (int)acAutomaton->trieSize - 1 && minFrequency > acNodeList[ptFirst]->frequency + acNodeList[ptFirst + 1]->frequency) {
				minFrequency = acNodeList[ptFirst]->frequency + acNodeList[ptFirst + 1]->frequency;
				operation = 1;
			}
			if (ptSecond < (int)hfmBranchNode.size() - 1 && minFrequency > getNode(hfmBranchNode[ptSecond])->weight + getNode(hfmBranchNode[ptSecond + 1])->weight) {
				minFrequency = getNode(hfmBranchNode[ptSecond])->weight + getNode(hfmBranchNode[ptSecond + 1])->weight;
				operation = 2;
			}
			if (ptFirst < acAutomaton->trieSize && ptSecond < (int)hfmBranchNode.size() && minFrequency > acNodeList[ptFirst]->frequency + getNode(hfmBranchNode[ptSecond])->weight) {
				minFrequency = acNodeList[ptFirst]->frequency + getNode(hfmBranchNode[ptSecond])->weight;
				operation = 3;
			}
			switch (operation) {
				case 1: {
					HuffmanLeafNode* hfmNode1 = new HuffmanLeafNode(huffmanSize, acNodeList[ptFirst]->id, acNodeList[ptFirst]->frequency, Config::NULL_POINTER, Config::NULL_POINTER);
					HuffmanLeafNode* hfmNode2 = new HuffmanLeafNode(huffmanSize + 1, acNodeList[ptFirst + 1]->id, acNodeList[ptFirst + 1]->frequency, Config::NULL_POINTER, Config::NULL_POINTER);
					HuffmanNode* hfmNode3 = new HuffmanNode(huffmanSize + 2, minFrequency, hfmNode1->id, hfmNode2->id);
					hfmNode1->father = hfmNode3->id;
					hfmNode2->father = hfmNode3->id;
					hfmBranchNode.push_back(hfmNode3->id);
					huffman->push_back(hfmNode1);
					huffman->push_back(hfmNode2);
					huffman->push_back(hfmNode3);
					huffmanSize += 3;
					ptFirst += 2;
					break;
				}
				case 2: {
					HuffmanNode* hfmNode3 = new HuffmanNode(huffmanSize, minFrequency, hfmBranchNode[ptSecond], hfmBranchNode[ptSecond + 1]);
					getNode(hfmBranchNode[ptSecond])->father = hfmNode3->id;
					getNode(hfmBranchNode[ptSecond + 1])->father = hfmNode3->id;
					hfmBranchNode.push_back(hfmNode3->id);
					huffman->push_back(hfmNode3);
					huffmanSize ++;
					ptSecond += 2;
					break;
				}
				case 3: {
					HuffmanLeafNode* hfmNode1 = new HuffmanLeafNode(huffmanSize, acNodeList[ptFirst]->id, acNodeList[ptFirst]->frequency, Config::NULL_POINTER, Config::NULL_POINTER);
					HuffmanNode* hfmNode2 = getNode(hfmBranchNode[ptSecond]);
					HuffmanNode* hfmNode3 = new HuffmanNode(huffmanSize + 1, minFrequency, hfmNode1->id, hfmNode2->id);
					hfmNode1->father = hfmNode3->id;
					hfmNode2->father = hfmNode3->id;
					hfmBranchNode.push_back(hfmNode3->id);
					huffman->push_back(hfmNode1);
					huffman->push_back(hfmNode3);
					huffmanSize += 2;
					ptFirst ++;
					ptSecond ++;
				}
				default: {
					this->root = getNode(huffmanSize - 1)->id;
				}
			}
		}
		
		hfmBranchNode.clear();
	}
	
	HuffmanNode* getNode(int id) {
		if (this->huffman->at(id)->id == id) {
			return this->huffman->at(id);
		}
		for (int i = 0; i < this->huffman->size(); ++i) {
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
		if (trieHuffmanCode != NULL) {
			delete[] trieHuffmanCode;
		}
	}
};

#endif
