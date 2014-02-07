//
//  utility.cpp
//  PRESS-v2
//
//  Created by Renchu Song on 6/2/14.
//  Copyright (c) 2014 Renchu Song. All rights reserved.
//

#include "utility.h"
#include <cmath>

double ecldDistance(EcldPoint* u, EcldPoint* v) {
	return sqrt((u->x - v->x) * (u->x - v->x) + (u->y - v->y) * (u->y - v->y));
}