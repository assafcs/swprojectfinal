#include "SPPoint.h"
#include <stdio.h>
#include <assert.h>

typedef struct sp_point_t{
	double* pData;
	int index;
	int dim;
};

SPPoint spPointCreate(double* data, int dim, int index) {
	if (data == NULL || dim == NULL || dim <= 0 || index == NULL || index < 0){
		return NULL;
	}
	SPPoint newPoint;
	newPoint->pData = (double*) malloc (dim * sizeof(double));
	if (newPoint->pData == NULL){
		return NULL;
	}
	int i;
	for (i = 0; i < dim; i++){
		newPoint->pData[i] = data[i];
	}
	newPoint->index = index;
	newPoint->dim = dim;
	return newPoint;

}

SPPoint spPointCopy(SPPoint source) {
	// assert true that source != source
	SPPoint copyPoint = spPointCreate(source->pData, source->dim, source->index);
	return copyPoint;
}

void spPointDestroy(SPPoint point) {
	if (point != NULL){
		free(point->pData);
	}
}

int spPointGetDimension(SPPoint point)  {
	//assert point != NULL
	return point->dim;
}

int spPointGetIndex(SPPoint point) {
	//assert point != NULL
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis) {
	// assert point != NULL and axis < point->dim
	return point->pData[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q) {

}


