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
#include "../SPKDTree.h"

static bool innerNodeState(SPKDTreeNode treeNode, int expectedDimension, double expectedMedian);
static bool leafNodeState(SPKDTreeNode treeNode, SPPoint expectedData);

// TODO: Use array tests method, and unify
static bool pointsEqualNotSame2(SPPoint aPoint, SPPoint bPoint);

SPPoint threeDimPoint(double x, double y, double z) {
	double *pointData = (double *) malloc(3 * sizeof(double));
	pointData[0] = x;
	pointData[1] = y;
	pointData[2] = z;
	return spPointCreate(pointData, 3, 0);
}

static bool kdTreeMaxSpreadProperBuildTest() {
	SPPoint *points = (SPPoint *) malloc(5 * sizeof(*points));
	points[0] = threeDimPoint(1, 60, -5.5);
	points[1] = threeDimPoint(123, 70, -4.5);
	points[2] = threeDimPoint(2, 80, 4.5);
	points[3] = threeDimPoint(9, 140.5, 7.5);
	points[4] = threeDimPoint(3, 8, 133.5);

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

	spDestroyKDArray(kdArray);
	// TODO: Destroy tree
	free(treeRoot);
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
	ASSERT(pointsEqualNotSame2(*(spKDTreeNodeGetData(treeNode)), expectedData));
	return true;
}

static bool pointsEqualNotSame2(SPPoint aPoint, SPPoint bPoint) {
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


int main() {
	RUN_TEST(kdTreeMaxSpreadProperBuildTest);
}
