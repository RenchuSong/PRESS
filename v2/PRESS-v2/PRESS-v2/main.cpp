//
//  main.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 4/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include <iostream>
#include "file_processor.h"
#include "trajectory.h"

using namespace std;

int main(int argc, const char * argv[])
{

    /*FileWriter* fw = new FileWriter("/Users/songrenchu/Develop/test/test.txt", false);
	fw->writeInt(123);
	fw->writeChar(' ');
	fw->writeDouble(21312123.53453434432);
	fw->writeChar('A');
	fw->writeChar(' ');
	fw->writeShort((short)101);
	fw->~FileWriter();
	
	FileReader* fr = new FileReader("/Users/songrenchu/Develop/test/test.txt", false);
	cout << fr->nextInt() << endl;
	cout << fr->nextDouble() << endl;
	cout << fr->nextChar() << endl;
	cout << fr->nextShort() << endl;*/
	
	GPSTrajectory* gps = new GPSTrajectory(new FileReader("/Users/songrenchu/Develop/test/test.txt", false));
	
	gps->store(new FileWriter("/Users/songrenchu/Develop/test/test2.txt", true));
	
    return 0;
}

