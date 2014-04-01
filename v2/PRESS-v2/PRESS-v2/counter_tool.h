//
//  counter_tool.h
//  PRESS-v2
//
//  Created by Renchu Song on 31/3/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_counter_tool_h
#define PRESS_v2_counter_tool_h

// This is a singleton class to do experiments
// Count experiment related results for analysis

class CounterTool {
private:
	static CounterTool* instance;
	CounterTool() {
		for (int i = 0; i < 100; ++i) {
			lenFrequency[i] = 0;
		}
	}
	
public:
	int lenFrequency[100];	// the frequency of each length of FST used during FST compression
	int fstFrequency[100];	// the frequency of each length of FST in Huffman tree
	
	static CounterTool* getInstance() {
		if (instance == NULL) {
			instance = new CounterTool();
		}
		return instance;
	}
	
	// reset
	void reset() {
		for (int i = 0; i < 100; ++i) {
			lenFrequency[i] = 0;
		}
		for (int i = 0; i < 100; ++i) {
			fstFrequency[i] = 0;
		}
		
	}
	
};

CounterTool* CounterTool::instance = NULL;


#endif
