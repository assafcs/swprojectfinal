/*
 * sp_kd_array_test.c
 *
 *  Created on: Jul 18, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include "unit_test_util.h"
#include "../SPKDArray.h"

static bool pointsArrayEqualNotSame(SPPoint* aPointsArray, SPPoint *bPointsArray, int size);
static bool pointsEqualNotSame(SPPoint aPoint, SPPoint bPoint);
static bool kdArrayState(SPKDArray kdArray, SPPoint *expectedPointsArray, int expectedSize, int expectedPointDimension);

SPPoint twoDPoint(double x, double y) {
	double *pointData = (double *) malloc(2 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	return spPointCreate(pointData, 2, 0);
}

static bool kdArrayInitTest() {
	SPPoint *points = (SPPoint *) malloc(3 * sizeof(*points));
	points[0] = twoDPoint(4, 7);
	points[1] = twoDPoint(3, 8);
	points[2] = twoDPoint(3.5, 7.5);

	SPKDArray kdArray = spKDArrayInit(points, 3);
	ASSERT(kdArrayState(kdArray, points, 3, 2));

	freePointsArray(points, 3);
	spDestroyKDArray(kdArray);
	return true;
}

static bool kdArraySplitTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = twoDPoint(1, 2);
	points[1] = twoDPoint(123, 70);
	points[2] = twoDPoint(2, 7);
	points[3] = twoDPoint(9, 11);
	points[4] = twoDPoint(3, 4);

	SPKDArray kdArray = spKDArrayInit(points, 5);
	ASSERT(kdArrayState(kdArray, points, 5, 2));

	SPKDArraySplitResult splitResult = spKDArraySplit(kdArray, 0);

	SPPoint *leftPoints = (SPPoint *) malloc(3 * sizeof(*leftPoints));
	leftPoints[0] = points[0];
	leftPoints[1] = points[2];
	leftPoints[2] = points[4];

	SPPoint *rightPoints = (SPPoint *) malloc(2 * sizeof(*rightPoints));
	rightPoints[0] = points[3];
	rightPoints[1] = points[1];

	ASSERT(kdArrayState(spKDArraySplitResultGetLeft(splitResult), leftPoints, 3, 2));
	ASSERT(kdArrayState(spKDArraySplitResultGetRight(splitResult), rightPoints, 2, 2));

	freePointsArray(points, 5);
	free(leftPoints);
	free(rightPoints);

	spDestroyKDArray(kdArray);
	spDestroyKDArraySplitResult(splitResult);
	return true;
}

/*** Helper assertion methods ***/

static bool kdArrayState(SPKDArray kdArray, SPPoint *expectedPointsArray, int expectedSize, int expectedPointDimension) {
	int axis, pointIndex, currentIndex;
	double currentValue, previousValue;
	int **indicesMatrix = spKDArrayGetIndicesMatrix(kdArray);

	ASSERT_NOT_NULL(kdArray);
	ASSERT_SAME(spKDArrayGetSize(kdArray), expectedSize);
	ASSERT(pointsArrayEqualNotSame(spKDArrayGetPointsArray(kdArray), expectedPointsArray, expectedSize));

	for (axis = 0; axis < expectedPointDimension; axis++) {
		previousValue = -DBL_MAX;
		for (pointIndex = 0; pointIndex < expectedSize; pointIndex++) {
			currentIndex = indicesMatrix[axis][pointIndex];
			currentValue = spPointGetAxisCoor(expectedPointsArray[currentIndex], axis);
			ASSERT_TRUE(currentValue >= previousValue);
			previousValue = currentValue;
		}
	}
	return true;
}

static bool pointsArrayEqualNotSame(SPPoint* aPointsArray, SPPoint *bPointsArray, int size) {
	int i;
	ASSERT_NOT_SAME(aPointsArray, bPointsArray);
	for (i = 0; i < size; i++) {
		ASSERT(pointsEqualNotSame(aPointsArray[i], bPointsArray[i]));
	}
	return true;
}

static bool pointsEqualNotSame(SPPoint aPoint, SPPoint bPoint) {
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

int main3() {
	RUN_TEST(kdArrayInitTest);
	RUN_TEST(kdArraySplitTest);
}
