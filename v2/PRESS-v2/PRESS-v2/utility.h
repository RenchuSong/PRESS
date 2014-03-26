//
//  utility.h
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef __PRESS_v2__utility__
#define __PRESS_v2__utility__

// euclidean point
struct EcldPoint {
	double x, y;
	EcldPoint(double x, double y) {
		this->x = x;
		this->y = y;
	}
};

// calculate the euclidean distance of two points
extern double ecldDistance(EcldPoint*, EcldPoint*);

#endif /* defined(__PRESS_v2__utility__) */
