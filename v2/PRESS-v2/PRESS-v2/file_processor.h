//
//  file_processor.h
//  PRESS-v2
//
//  Created by Renchu Song on 4/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef __PRESS_v2__file_processor__
#define __PRESS_v2__file_processor__

#include <iostream>

//convertor between char and binary char
union Char2Binary {
	char value;
	unsigned char data[1];
};
union Char2Binary char2Binary;

//convertor between short and binary short
union Short2Binary {
	short value;
	unsigned char data[2];
};
union Short2Binary short2Binary;

//convertor between int and binary int
union Int2Binary {
	int value;
	unsigned char data[4];
};
union Int2Binary int2Binary;

//convertor between double and binary double
union Double2Binary {
	double value;
	unsigned char data[8];
};
union Double2Binary double2Binary;

extern class FileReader;
extern class FileWriter;

#endif /* defined(__PRESS_v2__file_processor__) */
