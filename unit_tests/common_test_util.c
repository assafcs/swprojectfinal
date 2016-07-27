/*
 * common_test_util.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */


#include <stdlib.h>
#include <stdbool.h>
#include "common_test_util.h"
#include "unit_test_util.h"

bool pointsArrayEqualNotSame(SPPoint* aPointsArray, SPPoint *bPointsArray, int size) {
	int i;
	ASSERT_NOT_SAME(aPointsArray, bPointsArray);
	for (i = 0; i < size; i++) {
		ASSERT(pointsEqualNotSame(aPointsArray[i], bPointsArray[i]));
	}
	return true;
}

bool pointsEqualNotSame(SPPoint aPoint, SPPoint bPoint) {
	int i;
	int pointDimention = spPointGetDimension(aPoint);
	ASSERT_NOT_SAME(aPoint, bPoint);
	ASSERT_SAME(pointDimention, spPointGetDimension(bPoint));
	ASSERT_SAME(spPointGetIndex(aPoint), spPointGetIndex(bPoint));
	for (i = 0; i < pointDimention; i++) {
		ASSERT_SAME(spPointGetAxisCoor(aPoint, i), spPointGetAxisCoor(bPoint, i));
	}
	return true;
}

SPPoint twoDPoint(double x, double y) {
	double *pointData = (double *) malloc(2 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	return spPointCreate(pointData, 2, 0);
}

SPPoint threeDPoint(double x, double y, double z) {
	double *pointData = (double *) malloc(3 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	pointData[2] = z;
	return spPointCreate(pointData, 3, 0);
}
