//
//  test_base.h
//  PRESS-v2
//
//  Created by Renchu Song on 4/8/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_test_base_h
#define PRESS_v2_test_base_h

void isTrue(bool value) {
	if (value != true) throw "Expected true, false given.";
}


void isFalse(bool value) {
	if (value != false) throw "Expected false, true given.";
}

template <typename T>
void equals(T value, T expected) {
	if (value != expected) throw "Expected " + expected +", " + value + " given.";
}

template <typename T>
void equalsInOrder(T* value, int size, T* expected) {
	for (int i = 0; i < size; ++i) {
		if (value[i] != expected[i]) throw "At Index " + i + ":Expected " + expected +", " + value + " given.";
	}
}

#endif
