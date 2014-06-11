//
//  utility.h
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#ifndef __PRESS_v2__utility__
#define __PRESS_v2__utility__

#include <vector>

using namespace std;

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

// calculate the distance from a point to an interval
extern double point2IntervalDistance(EcldPoint*, EcldPoint*, EcldPoint*);

// Calculate the bias from start of a vector of points to the projection of a point on the vector of points
extern double edgeBias(EcldPoint*, vector<EcldPoint*>);

// Calculate the angle of two points
extern double angle(double x1, double y1, double x2, double y2);

// Interpolate values
extern double interpolate(double x, double y, double t);

// Return the distance bias from the start of a directed broken line
extern double bias(vector<EcldPoint*> geometry, EcldPoint* point);

#endif /* defined(__PRESS_v2__utility__) */
