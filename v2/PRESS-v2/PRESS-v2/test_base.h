//
//  test_base.h
//  PRESS-v2
//
//  Created by Renchu Song on 4/8/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_test_base_h
#define PRESS_v2_test_base_h

template <typename T>
void assert(T value, void* matcher) {
	try {
		matcher(value);
	} catch (e) {
		throw "Assertion Failed: " + e;
	}
}

void isTrue(bool value) {
	if (value != true) throw "Expected true, false given.";
}

#endif
