/*
 * sp_algorithms.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include "sp_algorithms.h"
#include <math.h>

void spKNearestNeighbours(SPKDTreeNode tree, SPBPQueue queue, SPPoint point) {
	int dim;
	double pointValue, nodeMedianValue, maxQueueValue;
	SPListElement insertedElement;
	SPPoint *leafPoint;
	if (tree == NULL || queue == NULL) {
		return;
	}
	if (spKDTreeNodeIsLeaf(tree)) {
		leafPoint = spKDTreeNodeGetData(tree);
		insertedElement = spListElementCreate(spPointGetIndex(*leafPoint), spPointL2SquaredDistance(*leafPoint, point));
		spBPQueueEnqueue(queue, insertedElement);
		return;
	}
	dim = spKDTreeNodeGetDimension(tree);
	nodeMedianValue = spKDTreeNodeGetMedianValue(tree);
	pointValue = spPointGetAxisCoor(point, dim);

	if (pointValue <= nodeMedianValue) {
		spKNearestNeighbours(spKDTreeNodeGetLeftChild(tree), queue, point);
	} else {
		spKNearestNeighbours(spKDTreeNodeGetRightChild(tree), queue, point);
	}
	maxQueueValue = spBPQueueMaxValue(queue);

	if (!spBPQueueIsFull(queue) || fabs(pointValue - nodeMedianValue) < maxQueueValue) {
		if (pointValue <= nodeMedianValue) {
			spKNearestNeighbours(spKDTreeNodeGetRightChild(tree), queue, point);
		} else {
			spKNearestNeighbours(spKDTreeNodeGetLeftChild(tree), queue, point);
		}
	}
}
