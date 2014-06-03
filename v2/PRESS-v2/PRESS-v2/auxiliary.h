//
//  auxiliary.h
//  PRESS-v2
//
//  Created by Renchu Song on 2/6/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef PRESS_v2_auxiliary_h
#define PRESS_v2_auxiliary_h

class MBR {
public:
	double x1, x2, y1, y2;
	
	MBR(double x1, double x2, double y1, double y2) {
		this->x1 = x1;
		this->x2 = x2;
		this->y1 = y1;
		this->y2 = y2;
	}
	
	bool intersect(MBR* other) {
		return (this->x1 < other->x2 && this->x2 > other->x1 && this->y1 < other->y2 && this->y2 > other->y1);
	}
	
	
};

#endif
