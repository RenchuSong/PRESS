//
//  utility.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//
// All global independent functions not included in specific classes should be put here

#include "utility.h"
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// Calculate the Euclidean distance of two Euclidean space points
double ecldDistance(EcldPoint* u, EcldPoint* v) {
	return sqrt((u->x - v->x) * (u->x - v->x) + (u->y - v->y) * (u->y - v->y));
}

// Calculate the Euclidean distance from one point to one interval
double vectorProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = u2->x - u1->x;
	double y1 = u2->y - u1->y;
	double x2 = v2->x - v1->x;
	double y2 = v2->y - v1->y;
	
	return x1 * y2 - x2 * y1;
}

double scalarProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = u2->x - u1->x;
	double y1 = u2->y - u1->y;
	double x2 = v2->x - v1->x;
	double y2 = v2->y - v1->y;
	
	return x1 * x2 + y1 * y2;
}

double point2Interval(EcldPoint* p, EcldPoint* pre, EcldPoint* scc) {
	double result = min(ecldDistance(p, pre), ecldDistance(p, scc));
	if (scalarProduct(pre, p, pre, scc) > 0 && scalarProduct(scc, p, scc, pre) > 0) {
		result = min(result, fabs(vectorProduct(pre, scc, pre, p) / ecldDistance(scc, pre)));
	}
	return result;
}