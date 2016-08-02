/*
 * SPKDArray.c
 *
 *  Created on: Jul 17, 2016
 *      Author: mataneilat
 */

#include "SPKDArray.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct index_to_value {
	int index;
	double value;
} IndexToValue;

struct sp_kd_array_t {
	int **indicesMatrix;
	SPPoint* pointsArray;
	int size;
};

struct sp_split_result_t {
	SPKDArray left;
	SPKDArray right;
};

typedef struct split_index_mapping {
	int arrIdentifier; // 0 is left, 1 is right
	int index;
} SplitIndexMapping;

SPPoint *copyPointsArray(SPPoint *pointsArray, int size);
int **createIndicesMatrix(SPPoint *pointsArray, int size, int pointsDimenstion);
void freeIndicesMatrix(int **indicesMatrix, int rows);

int **spKDArrayGetIndicesMatrix(SPKDArray kdArray) {
	if (kdArray == NULL) return NULL;
	return kdArray->indicesMatrix;
}

SPPoint *spKDArrayGetPointsArray(SPKDArray kdArray) {
	if (kdArray == NULL) return NULL;
	return kdArray->pointsArray;
}

int spKDArrayGetSize(SPKDArray kdArray) {
	if (kdArray == NULL) return -1;
	return kdArray->size;
}

int comparePointsCoodinates(const void *aIndexToValuePtr, const void *bIndexToValuePtr) {
	IndexToValue *aIndexToValue = (IndexToValue *) aIndexToValuePtr;
	IndexToValue *bIndexToValue = (IndexToValue *) bIndexToValuePtr;
	if (aIndexToValue->value == bIndexToValue->value) {
		return 0;
	}
	return (aIndexToValue->value < bIndexToValue->value) ? -1 : 1;
}

int *sortedIndices(SPPoint *pointsArray, int size, int axis) {
	int i;
	IndexToValue indexToValue;
	IndexToValue *indicesToValues = (IndexToValue *) malloc(sizeof(IndexToValue) * size);
	int *indicesArray = (int *) malloc (sizeof(int) * size);
	if (indicesToValues == NULL || indicesArray == NULL) {
		return NULL;
	}
	for (i = 0; i < size; i++) {
		indexToValue = (IndexToValue){i, spPointGetAxisCoor(pointsArray[i], axis)};
		indicesToValues[i] = indexToValue;
	}
	qsort(indicesToValues, size, sizeof(IndexToValue), comparePointsCoodinates);

	for (i = 0; i < size; i++) {
		indicesArray[i] = indicesToValues[i].index;
	}
	free(indicesToValues);
	return indicesArray;
}

SPKDArray spKDArrayInit(SPPoint *arr, int size) {
	if (arr == NULL || size <= 0) {
		return NULL;
	}
	int pointsDimension = spPointGetDimension(arr[0]);
	SPKDArray kdArray = (SPKDArray ) malloc(sizeof(*kdArray));
	if (kdArray == NULL) {
		return NULL;
	}
	int **indicesMatrix = createIndicesMatrix(arr, size, pointsDimension);
	if (indicesMatrix == NULL) {
		spKDArrayDestroy(kdArray);
		return NULL;
	}
	SPPoint *arrCopy = copyPointsArray(arr, size);
	if (arrCopy == NULL) {
		spKDArrayDestroy(kdArray);
		return NULL;
	}

	kdArray->pointsArray = arrCopy;
	kdArray->indicesMatrix = indicesMatrix;
	kdArray->size = size;

	return kdArray;
}

int **createIndicesMatrix(SPPoint *pointsArray, int size, int pointsDimension) {
	int i;
	int *axisIndices;
	int **indicesMatrix = (int **) malloc(sizeof(*indicesMatrix) * pointsDimension);
	if (indicesMatrix == NULL) {
		return NULL;
	}
	for (i = 0; i < pointsDimension; i++) {
		axisIndices = sortedIndices(pointsArray, size, i);
		if (axisIndices == NULL) {
			freeIndicesMatrix(indicesMatrix, pointsDimension);
			return NULL;
		}
		indicesMatrix[i] = axisIndices;
	}
	return indicesMatrix;
}

SPPoint *copyPointsArray(SPPoint *pointsArray, int size) {
	int i;
	SPPoint pointCopy;
	SPPoint *arrCopy = (SPPoint *) malloc(sizeof(*pointsArray) * size);
	if (arrCopy == NULL) {
		return NULL;
	}
	for (i = 0; i < size; i++) {
		pointCopy = spPointCopy(pointsArray[i]);
		if (pointCopy == NULL) {
			freePointsArray(arrCopy, size);
			return NULL;
		}
		arrCopy[i] = pointCopy;
	}
	return arrCopy;
}

SPKDArraySplitResult spKDArraySplit(SPKDArray kdArr, int coor) {
	if (kdArr == NULL || coor < 0 || coor > spKDArrayGetPointsDimension(kdArr)) {
		return NULL;
	}
	int i, j, currentIndex, leftPointIndex, rightPointIndex, currentArrIdentifier;
	SPPoint pointCopy;
	SPKDArray currentArr;
	SplitIndexMapping currentIndexMapping;
	int *leftIndices, *rightIndices, *currentPointIndex;
	int pointsDimension = spKDArrayGetPointsDimension(kdArr);
	int size = kdArr->size;
	int **indicesMatrix = kdArr->indicesMatrix;
	int *sortedIndices = indicesMatrix[coor];

	SplitIndexMapping *indexMapping = (SplitIndexMapping *) malloc(size * sizeof(SplitIndexMapping));
	if (indexMapping == NULL) {
		return NULL;
	}

	SPKDArraySplitResult splitResult = (SPKDArraySplitResult) malloc(sizeof(*splitResult));
	splitResult->left = (SPKDArray) malloc(sizeof(*(splitResult->left)));
	splitResult->right = (SPKDArray) malloc(sizeof(*(splitResult->right)));

	if (splitResult->left == NULL || splitResult->right == NULL) {
		free(indexMapping);
		spKDArraySplitResultDestroy(splitResult);
		return NULL;
	}

	int leftCount = (int)ceil((double)kdArr->size / 2.0);
	int rightCount = (int)floor((double)kdArr->size / 2.0);

	splitResult->left->size = leftCount;
	splitResult->right->size = rightCount;

	splitResult->left->pointsArray = (SPPoint *) malloc(leftCount * sizeof(SPPoint));
	splitResult->right->pointsArray = (SPPoint *) malloc(rightCount * sizeof(SPPoint));

	splitResult->left->indicesMatrix = (int **) malloc(pointsDimension * sizeof(int *));
	splitResult->right->indicesMatrix = (int **) malloc(pointsDimension * sizeof(int *));

	if (splitResult->left->pointsArray == NULL || splitResult->right->pointsArray == NULL
			|| splitResult->left->indicesMatrix == NULL || splitResult->right->indicesMatrix == NULL) {
		free(indexMapping);
		spKDArraySplitResultDestroy(splitResult);
		return NULL;
	}

	leftPointIndex = 0;
	rightPointIndex = 0;
	for (i = 0; i < size; i++) {
		currentIndex = sortedIndices[i];
		if (i < leftCount) {
			currentArr = splitResult->left;
			currentPointIndex = &leftPointIndex;
			currentArrIdentifier = 0;
		} else {
			currentArr = splitResult->right;
			currentPointIndex = &rightPointIndex;
			currentArrIdentifier = 1;
		}
		pointCopy = spPointCopy(kdArr->pointsArray[currentIndex]);
		currentArr->pointsArray[*currentPointIndex] = pointCopy;
		indexMapping[currentIndex] = (SplitIndexMapping) {currentArrIdentifier, *currentPointIndex};
		(*currentPointIndex)++;
	}

	for (i = 0; i < pointsDimension; i++) {
		leftIndices = (int *) malloc(leftCount * sizeof(int));
		rightIndices = (int *) malloc(rightCount * sizeof(int));

		if (leftIndices == NULL || rightIndices == NULL) {
			free(indexMapping);
			spKDArraySplitResultDestroy(splitResult);
			return NULL;
		}
		splitResult->left->indicesMatrix[i] = leftIndices;
		splitResult->right->indicesMatrix[i] = rightIndices;

		leftPointIndex = 0;
		rightPointIndex = 0;
		for (j = 0; j < size; j++) {
			currentIndex = kdArr->indicesMatrix[i][j];
			currentIndexMapping = indexMapping[currentIndex];
			if (currentIndexMapping.arrIdentifier == 0) {
				currentArr = splitResult->left;
				currentPointIndex = &leftPointIndex;
			} else {
				currentArr = splitResult->right;
				currentPointIndex = &rightPointIndex;
			}
			currentArr->indicesMatrix[i][*currentPointIndex] = currentIndexMapping.index;
			(*currentPointIndex)++;
		}
	}

	free(indexMapping);

	return splitResult;
}

double spKDArrayGetSpread(SPKDArray kdArr, int coor) {
	int pointsDim, arrSize;
	int **indicesMatrix;
	int *sortedIndices = NULL;
	SPPoint minimalPoint, maximalPoint;
	if (kdArr == NULL) {
		return -1;
	}
	pointsDim = spKDArrayGetPointsDimension(kdArr);
	arrSize = kdArr->size;
	indicesMatrix = kdArr->indicesMatrix;
	if (arrSize <= 0) {
		// Empty array..
		return -1;
	}
	if (coor < 0 || coor > pointsDim) {
		// Invalid argument
		return -1;
	}
	sortedIndices = indicesMatrix[coor];
	minimalPoint = kdArr->pointsArray[sortedIndices[0]];
	maximalPoint = kdArr->pointsArray[sortedIndices[arrSize - 1]];
	return spPointGetAxisCoor(maximalPoint, coor) - spPointGetAxisCoor(minimalPoint, coor);
}

double spKDArrayGetMedian(SPKDArray kdArr, int coor) {
	int pointsDim, arrSize;
	int **indicesMatrix;
	int *sortedIndices = NULL;
	SPPoint medianPoint;
	if (kdArr == NULL) {
		return -1;
	}
	pointsDim = spKDArrayGetPointsDimension(kdArr);
	arrSize = kdArr->size;
	indicesMatrix = kdArr->indicesMatrix;
	if (arrSize <= 0) {
		// Empty array..
		return -1;
	}
	if (coor < 0 || coor > pointsDim) {
		// Invalid argument
		return -1;
	}
	sortedIndices = indicesMatrix[coor];
	medianPoint = kdArr->pointsArray[sortedIndices[(arrSize - 1) / 2]];
	return spPointGetAxisCoor(medianPoint, coor);
}

int spKDArrayMaxSpreadDimension(SPKDArray kdArr) {
	int dim, currentDimension, maxSpreadDimension = -1;
	double currentSpread, maxSpread = -1;
	if (kdArr == NULL) {
		return -1;
	}
	dim = spKDArrayGetPointsDimension(kdArr);
	if (dim <= 0) {
		return -1;
	}
	for (currentDimension = 0; currentDimension < dim; currentDimension++) {
		currentSpread = spKDArrayGetSpread(kdArr, currentDimension);
		if (currentSpread > maxSpread) {
			maxSpread = currentSpread;
			maxSpreadDimension = currentDimension;
		}
	}
	return maxSpreadDimension;
}

int spKDArrayGetPointsDimension(SPKDArray kdArray) {
	if (kdArray == NULL || kdArray->size == 0) return -1;
	return spPointGetDimension(kdArray->pointsArray[0]);
}

void freeIndicesMatrix(int **indicesMatrix, int rows) {
	int i;
	int *row;
	if (indicesMatrix == NULL) {
		return;
	}
	for (i = 0; i < rows; i++) {
		row = indicesMatrix[i];
		if (row != NULL) {
			free(row);
		}
	}
	free(indicesMatrix);
}

void spKDArrayDestroy(SPKDArray kdArray) {
	if (kdArray == NULL) {
		return;
	}
	// Order here is important, since the dimension relies on the points array..
	freeIndicesMatrix(kdArray->indicesMatrix, spKDArrayGetPointsDimension(kdArray));
	freePointsArray(kdArray->pointsArray, kdArray->size);
	free(kdArray);
}

/*** Split result access methods ***/

SPKDArray spKDArraySplitResultGetLeft(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return NULL;
	}
	return splitResult->left;
}

SPKDArray spKDArraySplitResultGetRight(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return NULL;
	}
	return splitResult->right;
}

void spKDArraySplitResultDestroy(SPKDArraySplitResult splitResult) {
	if (splitResult == NULL) {
		return;
	}
	spKDArrayDestroy(splitResult->left);
	spKDArrayDestroy(splitResult->right);
	free(splitResult);
}

