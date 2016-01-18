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
	
	vector<bool> code;	// huffman code buffer
	// assign huffman code
	void assignHuffmanCode(int trieId) {
		this->trieHuffmanCode[trieId] = new HuffmanCode();
		for (int i = 0; i < code.size(); ++i) {
			this->trieHuffmanCode[trieId]->code.push_back(code[i]);
		}
	}
	// recursively get huffman code
	void getHuffmanCode(HuffmanNode* node) {
		if (node->leftSon == Config::NULL_POINTER && node->rightSon == Config::NULL_POINTER) {
			assignHuffmanCode(((HuffmanLeafNode*)node)->trieId);
			return;
		}
		code.push_back(false);
		getHuffmanCode(getNode(node->leftSon));
		code.pop_back();
		
		code.push_back(true);
		getHuffmanCode(getNode(node->rightSon));
		code.pop_back();
	}
	
public:
	int huffmanSize, trieSize;
	vector<HuffmanNode*>* huffman = new vector<HuffmanNode*>();
	HuffmanCode** trieHuffmanCode = NULL;	// each trie node's huffman code
	int root;
	
	HuffmanTree(ACAutomaton* acAutomaton) {
		if (acAutomaton == NULL) {
			throw "AC Automaton NULL exception";
		}
		this->trieSize = acAutomaton->trieSize;
		trieHuffmanCode = new HuffmanCode*[acAutomaton->trieSize];		// the huffman code of each trie node
		ACNode** acNodeList = new ACNode*[acAutomaton->trieSize];
		for (int i = 0; i < acAutomaton->trieSize; ++i) {
			acNodeList[i] = acAutomaton->trie->at(i);
		}
		
		sort(acNodeList, acNodeList + acAutomaton->trieSize, cmp);
		
		vector<int> hfmBranchNode;
		
		int ptFirst = 0, ptSecond = 0;
		huffmanSize = 0;
		cout << "total: " << acAutomaton->trieSize << endl;
		for (int i = 0; i < acAutomaton->trieSize; ++i) {
			if (i % 10000 == 0) {
				cout << i << "\t";
			}
			
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
		
		cout << endl;
		
		hfmBranchNode.clear();
		
		// get huffman code
		getHuffmanCode(getNode(root));
	}
	
	HuffmanTree(FileReader* fr) {
		huffmanSize = fr->nextInt();
		trieSize = fr->nextInt();
		root = fr->nextInt();
		for (int i = 0; i < huffmanSize; ++i) {
			int id = fr->nextInt();
			int weight = fr->nextInt();
			int father = fr->nextInt();
			int leftSon = fr->nextInt();
			int rightSon = fr->nextInt();
			int trieId = fr->nextInt();
			HuffmanNode* node;
			if (trieId == Config::NULL_POINTER) {
				node = new HuffmanNode(id, weight, leftSon, rightSon);
			} else {
				node = new HuffmanLeafNode(id, trieId, weight, leftSon, rightSon);
			}
			node->father = father;
			huffman->push_back(node);
		}
		
		trieHuffmanCode = new HuffmanCode*[trieSize];		// the huffman code of each trie node
		
		// get huffman code
		getHuffmanCode(getNode(root));
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
	
	// store the Huffman tree structure
	void store(FileWriter* fw) {
		fw->writeInt(huffmanSize);
		fw->writeInt(trieSize);
		fw->writeInt(root);
		for (int i = 0; i < huffman->size(); ++i) {
			fw->writeInt(huffman->at(i)->id);
			fw->writeInt(huffman->at(i)->weight);
			fw->writeInt(huffman->at(i)->father);
			fw->writeInt(huffman->at(i)->leftSon);
			fw->writeInt(huffman->at(i)->rightSon);
			
			if (huffman->at(i)->leftSon == Config::NULL_POINTER && huffman->at(i)->rightSon == Config::NULL_POINTER) {
				fw->writeInt(((HuffmanLeafNode*)huffman->at(i))->trieId);
			} else {
				fw->writeInt(Config::NULL_POINTER);
			}
		}
	}
	
	void display() {
		display(getNode(root));
	}
	
	void displayCode() {
		for (int i = 0; i < this->trieSize; ++i) {
			cout << "Trie " << i << ": ";
			for (int j = 0; j < trieHuffmanCode[i]->code.size(); ++j) {
				cout << trieHuffmanCode[i]->code[j];
			}
			cout << endl;
		}
	}
	
	~HuffmanTree() {
		if (trieHuffmanCode != NULL) {
			delete[] trieHuffmanCode;
		}
	}
};

#endif
