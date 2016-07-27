/*
 * sp_kd_tree_unit_test.c
 *
 *  Created on: Jul 26, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include "unit_test_util.h"
#include "common_test_util.h"
#include "../SPKDTree.h"

static bool innerNodeState(SPKDTreeNode treeNode, int expectedDimension, double expectedMedian);
static bool leafNodeState(SPKDTreeNode treeNode, SPPoint expectedData);

static bool kdTreeMaxSpreadProperBuildTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = threeDPoint(1, 60, -5.5);
	points[1] = threeDPoint(123, 70, -4.5);
	points[2] = threeDPoint(2, 80, 4.5);
	points[3] = threeDPoint(9, 140.5, 7.5);
	points[4] = threeDPoint(3, 8, 133.5);

	SPKDArray kdArray = spKDArrayInit(points, 5);

	SPKDTreeNode treeRoot = spKDTreeBuild(kdArray, TREE_SPLIT_METHOD_MAX_SPREAD);

	ASSERT(innerNodeState(treeRoot, 2, 4.5));

	SPKDTreeNode leftTree = spKDTreeNodeGetLeftChild(treeRoot);

	ASSERT(innerNodeState(leftTree, 0, 2));

	SPKDTreeNode leftTreeLeftChild = spKDTreeNodeGetLeftChild(leftTree);

	ASSERT(innerNodeState(leftTreeLeftChild, 1, 60));

	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTree), points[1]));

	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(leftTreeLeftChild), points[0]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(leftTreeLeftChild), points[2]));

	SPKDTreeNode rightTree = spKDTreeNodeGetRightChild(treeRoot);

	ASSERT(innerNodeState(rightTree, 1, 8));

	ASSERT(leafNodeState(spKDTreeNodeGetLeftChild(rightTree), points[4]));
	ASSERT(leafNodeState(spKDTreeNodeGetRightChild(rightTree), points[3]));

	freePointsArray(points, 5);
	spKDArrayDestroy(kdArray);
	spKDTreeDestroy(treeRoot);
	return true;
}

static bool innerNodeState(SPKDTreeNode treeNode, int expectedDimension, double expectedMedian) {
	ASSERT_NOT_NULL(treeNode);
	ASSERT_SAME(spKDTreeNodeGetDimension(treeNode), expectedDimension);
	ASSERT_SAME(spKDTreeNodeGetMedianValue(treeNode), expectedMedian);
	ASSERT_NULL(spKDTreeNodeGetData(treeNode));
	return true;
}

static bool leafNodeState(SPKDTreeNode treeNode, SPPoint expectedData) {
	ASSERT_NOT_NULL(treeNode);
	ASSERT_SAME(spKDTreeNodeGetDimension(treeNode), -1);
	ASSERT_SAME(spKDTreeNodeGetMedianValue(treeNode), 0);
	ASSERT_NULL(spKDTreeNodeGetLeftChild(treeNode));
	ASSERT_NULL(spKDTreeNodeGetRightChild(treeNode));
	ASSERT(pointsEqualNotSame(*(spKDTreeNodeGetData(treeNode)), expectedData));
	return true;
}


int main() {
	RUN_TEST(kdTreeMaxSpreadProperBuildTest);
}
