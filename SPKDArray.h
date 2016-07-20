/*
 * SPKDArray.h
 *
 *  Created on: Jul 17, 2016
 *      Author: mataneilat
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

typedef struct sp_kd_array_t *SPKDArray;

typedef struct sp_split_result_t *SPKDArraySplitResult;

SPKDArray spKDArrayInit(SPPoint *arr, int size);

SPKDArraySplitResult spKDArraySplit(SPKDArray kdArr, int coor);

void spDestroyKDArray(SPKDArray kdArray);

/*** Split result access methods ***/

SPKDArray spKDArraySplitResultGetLeft(SPKDArraySplitResult splitResult);

SPKDArray spKDArraySplitResultGetRight(SPKDArraySplitResult splitResult);

void spDestroyKDArraySplitResult(SPKDArraySplitResult splitResult);

// For Test uses

int **spKDArrayGetIndicesMatrix(SPKDArray kdArray);

SPPoint *spKDArrayGetPointsArray(SPKDArray kdArray);

int spKDArrayGetSize(SPKDArray kdArray);

void freePointsArray(SPPoint *pointsArray, int size);

#endif /* SPKDARRAY_H_ */
