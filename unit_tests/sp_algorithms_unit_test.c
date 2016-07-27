/*
 * sp_algorithms_unit_test.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include "../sp_algorithms.h"
#include "common_test_util.h"
#include "unit_test_util.h"

static bool peekEqualsAndDequeue(SPBPQueue queue, int index, double value);

static bool spSimpleNearestNeighboutTest() {

	SPPoint searchedPoint = threeDPoint(20, 50, 100);

	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = indexedThreeDPoint(0, 1, 60, -5.5); // distance is 11591.25
	points[1] = indexedThreeDPoint(1, 123, 70, -4.5); // 21929.25
	points[2] = indexedThreeDPoint(2, 2, 80, 4.5); // 10344.25
	points[3] = indexedThreeDPoint(3, 9, 140.5, 7.5); // 16867.5
	points[4] = indexedThreeDPoint(4, 3, 8, 133.5); // 3175.25

	SPKDArray kdArray = spKDArrayInit(points, 5);

	// Assert for max spread split

	SPKDTreeNode treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_MAX_SPREAD);

	SPBPQueue queue = spBPQueueCreate(4);

	spKNearestNeighbours(treeRoot, queue, searchedPoint);

	ASSERT_TRUE(spBPQueueIsFull(queue));

	ASSERT(peekEqualsAndDequeue(queue, 4, 3175.25));
	ASSERT(peekEqualsAndDequeue(queue, 2, 10344.25));
	ASSERT(peekEqualsAndDequeue(queue, 0, 11591.25));
	ASSERT(peekEqualsAndDequeue(queue, 3, 16867.5));

	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	spKDTreeDestroy(treeRoot);

	// Assert for incremental split

	treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_INCREMENTAL);

	spKNearestNeighbours(treeRoot, queue, searchedPoint);

	ASSERT_TRUE(spBPQueueIsFull(queue));

	ASSERT(peekEqualsAndDequeue(queue, 4, 3175.25));
	ASSERT(peekEqualsAndDequeue(queue, 2, 10344.25));
	ASSERT(peekEqualsAndDequeue(queue, 0, 11591.25));
	ASSERT(peekEqualsAndDequeue(queue, 3, 16867.5));

	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	spKDTreeDestroy(treeRoot);

	// Assert for random split

	treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_RANDOM);

	spKNearestNeighbours(treeRoot, queue, searchedPoint);

	ASSERT_TRUE(spBPQueueIsFull(queue));

	ASSERT(peekEqualsAndDequeue(queue, 4, 3175.25));
	ASSERT(peekEqualsAndDequeue(queue, 2, 10344.25));
	ASSERT(peekEqualsAndDequeue(queue, 0, 11591.25));
	ASSERT(peekEqualsAndDequeue(queue, 3, 16867.5));

	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	freePointsArray(points, 5);
	spKDArrayDestroy(kdArray);
	spKDTreeDestroy(treeRoot);
	spBPQueueDestroy(queue);
	return true;
}

static bool peekEqualsAndDequeue(SPBPQueue queue, int index, double value) {
	SPListElement element = spBPQueuePeek(queue);
	ASSERT_SAME(spListElementGetIndex(element), index);
	ASSERT_SAME(spListElementGetValue(element), value);

	spBPQueueDequeue(queue);
	spListElementDestroy(element);
	return true;
}

int main() {
	RUN_TEST(spSimpleNearestNeighboutTest);
}
