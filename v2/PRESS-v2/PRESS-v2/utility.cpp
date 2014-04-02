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

double x2GoundTruth(double x1, double x2) {
	return (x2 - x1) * 111195.19;
}

double y2GoundTruth(double y1, double y2) {
	return (y2 - y1) * 111195.19;
}

// Calculate the Euclidean distance of two Euclidean space points
double ecldDistance(EcldPoint* u, EcldPoint* v) {
	return sqrt(x2GoundTruth(v->x, u->x) * x2GoundTruth(v->x, u->x) + y2GoundTruth(v->y, u->y) * y2GoundTruth(v->y, u->y));
}

// Calculate the Euclidean distance from one point to one interval
double vectorProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = x2GoundTruth(u1->x, u2->x);
	double y1 = y2GoundTruth(u1->y, u2->y);
	double x2 = x2GoundTruth(v1->x, v2->x);
	double y2 = y2GoundTruth(v1->y, v2->y);
	
	return x1 * y2 - x2 * y1;
}

double scalarProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = x2GoundTruth(u1->x, u2->x);
	double y1 = y2GoundTruth(u1->y, u2->y);
	double x2 = x2GoundTruth(v1->x, v2->x);
	double y2 = y2GoundTruth(v1->y, v2->y);
	
	return x1 * x2 + y1 * y2;
}

double point2IntervalDistance(EcldPoint* p, EcldPoint* pre, EcldPoint* scc) {
	double result = min(ecldDistance(p, pre), ecldDistance(p, scc));
	if (scalarProduct(pre, p, pre, scc) > 0 && scalarProduct(scc, p, scc, pre) > 0) {
		result = min(result, fabs(vectorProduct(pre, scc, pre, p) / ecldDistance(scc, pre)));
	}
	return result;
}

// Calculate the bias from start of a vector of points to the projection of a point on the vector of points
double edgeBias(EcldPoint* p, vector<EcldPoint*> geometry) {
	int flag = 0;
	double mid = 1e100;
	for (int i = 0; i < geometry.size() - 1; ++i) {
		double tmp = point2IntervalDistance(p, geometry[i], geometry[i + 1]);
		if (tmp < mid) {
			mid = tmp;
			flag = i;
		}
	}
	
	double result = 0;
	for (int i = 0; i < flag; ++i) {
		result += ecldDistance(geometry[i], geometry[i + 1]);
	}
	result += scalarProduct(geometry[flag], p, geometry[flag], geometry[flag + 1]) / ecldDistance(geometry[flag], geometry[flag + 1]);
	
	return result;
}

// Calculate the angle of two points
double angle(double x1, double y1, double x2, double y2) {
	if (x1 >= x2) {
		if (y1 < y2) {
			return 1e100;
		}
		return -1e100;
	}
	return (y2 - y1) / (x2 - x1);
}