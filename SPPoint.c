
#include "SPPoint.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define ZERO 0

struct sp_point_t {
	double* pData;
	int index;
	int dim;
};

SPPoint spPointCreate(double* data, int dim, int index) {
	assert(!(data == NULL || dim <= ZERO  || index < ZERO));

	SPPoint createdPoint = (SPPoint) malloc(sizeof(*createdPoint));
	if (createdPoint == NULL) {
		return NULL;
	}

	double* pointData = (double*) malloc(dim * sizeof(double));
	if (pointData == NULL) {
		free(createdPoint);
		return NULL;
	}

	int i;
	for (i = 0; i < dim; i++){
		pointData[i] = data[i];
	}

	createdPoint->pData = pointData;
	createdPoint->index = index;
	createdPoint->dim = dim;
	return createdPoint;

}

SPPoint spPointCopy(SPPoint source) {
	assert(source != NULL);
	SPPoint copyPoint = spPointCreate(source->pData, source->dim, source->index);
	return copyPoint;
}

void spPointDestroy(SPPoint point) {
	if (point != NULL){
		free(point->pData);
	}
	free(point);
}

int spPointGetDimension(SPPoint point)  {
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point) {
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis) {
	assert(point != NULL && axis < point->dim);
	return point->pData[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q) {
	assert(p != NULL && q != NULL && p->dim == q->dim);
	int i;
	double squared_dist = 0;
	for (i = 0; i < p->dim; i++){
		double diff = p->pData[i] - q->pData[i];
		squared_dist += (diff * diff);
	}
	return squared_dist;
}


