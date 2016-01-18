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

const double _NULL = -1;
const double _EBSILON = 1e-8;

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

// vector product of two vectors
double vectorProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = x2GoundTruth(u1->x, u2->x);
	double y1 = y2GoundTruth(u1->y, u2->y);
	double x2 = x2GoundTruth(v1->x, v2->x);
	double y2 = y2GoundTruth(v1->y, v2->y);
	
	return x1 * y2 - x2 * y1;
}

// scalar product of two vectors
double scalarProduct(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	double x1 = x2GoundTruth(u1->x, u2->x);
	double y1 = y2GoundTruth(u1->y, u2->y);
	double x2 = x2GoundTruth(v1->x, v2->x);
	double y2 = y2GoundTruth(v1->y, v2->y);
	
	return x1 * x2 + y1 * y2;
}

// judge if two vectors intersects
bool vectorIntersect(EcldPoint* u1, EcldPoint* u2, EcldPoint* v1, EcldPoint* v2) {
	return vectorProduct(u1, u2, u1, v1) * vectorProduct(u1, u2, u1, v2) < 0 &&
		   vectorProduct(v1, v2, v1, u1) * vectorProduct(v1, v2, v1, u2) < 0;
}

// Calculate the Euclidean distance from one point to one interval
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

// Interpolate values
double interpolate(double x, double y, double t) {
	return x * (1 - t) + y * t;
}

// Uninterpolate value
double unInterpolate(double x, double y, double a) {
	if (x == y) throw "Infinite value";
	return (a - x) / (y - x);
}

// y is within x
bool vectorImply(vector<double> x, vector<double> y) {
	if (x.size() != y.size()) return false;
	if (x.size() == 0) return true;
	bool flag = false;
	double r = 0;
	for (int i = 0; i < x.size(); ++i) {
		if (x[i] != 0) {
			r = y[i] / x[i];
			flag = true;
			break;
		}
	}
	if (!flag) {
		for (int i = 0; i < x.size(); ++i) {
			if (y[i] != 0) return false;
		}
		return true;
	}
	if (r > 1) return false;
	if (r < -_EBSILON) return false;
	for (int i = 0; i < x.size(); ++i) {
		if (y[i] == 0 && x[i] == 0) continue;
		if (fabs(y[i] / x[i] - r) > _EBSILON) return false;
	}
	return true;
}

// Return the distance bias from the start of a directed broken line
double bias(vector<EcldPoint*> geometry, EcldPoint* point) {
	double d = 0;
	for (int i = 1; i < geometry.size(); ++i) {
		double x1 = point->x - geometry[i - 1]->x,
			   x2 = geometry[i]->x - geometry[i - 1]->x,
			   y1 = point->y - geometry[i - 1]->y,
			   y2 = geometry[i]->y - geometry[i - 1]->y;
		double cross = x1 * y1 - x2 * y1,
			   dot = x1 * x2 + y1 * y2;
		if (fabs(cross) < _EBSILON) {
			d += dot / ecldDistance(geometry[i - 1], geometry[i]);
			return d;
		}
		d += ecldDistance(geometry[i - 1], geometry[i]);
	}
	return _NULL;
}

