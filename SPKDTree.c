/*
 * SPKDTree.c
 *
 *  Created on: Jul 19, 2016
 *      Author: mataneilat
 */

#include "SPKDTree.h"
#include <stdlib.h>

struct sp_kd_tree_node_t {
	int dim;
	double medianVal;
	struct sp_kd_tree_node_t *leftChild;
	struct sp_kd_tree_node_t *rightChild;
	SPPoint *data;
};

/*** Methods declarations ***/

SPKDTreeNode innerBuildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod, int previousSplitDimension);

SPKDTreeNode buildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod) {
	return innerBuildTree(kdArray, splitMethod, -1);
}

SPKDTreeNode innerBuildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod, int previousSplitDimension) {
	int arraySize, medianVal, splitDimension, maxDimension;
	SPKDTreeNode treeNode = NULL;
	SPKDArraySplitResult splitResult = NULL;
	if (kdArray == NULL) {
		return NULL;
	}
	arraySize = spKDArrayGetSize(kdArray);
	if (arraySize == 0) {
		return NULL;
	}
	if (arraySize == 1) {
		treeNode = (SPKDTreeNode) malloc (sizeof(*treeNode));
		treeNode->dim = -1;
		treeNode->medianVal = 0;
		treeNode->leftChild = NULL;
		treeNode->rightChild = NULL;
		treeNode->data = spKDArrayGetPointsArray(kdArray);
		return treeNode;
	}
	maxDimension = spKDArrayGetPointsDimension(kdArray);
	switch (splitMethod) {
	case TREE_SPLIT_METHOD_MAX_SPREAD:
		splitDimension = spKDArrayMaxSpreadDimension(kdArray);
		break;
	case TREE_SPLIT_METHOD_RANDOM:
		splitDimension = rand() % maxDimension;
		break;
	case TREE_SPLIT_METHOD_INCREMENTAL:
		splitDimension = (previousSplitDimension + 1) % maxDimension;
		break;
	}
	medianVal = spKDArrayGetMedian(kdArray, splitDimension);
	splitResult = spKDArraySplit(kdArray, splitDimension);
	treeNode->leftChild = buildTree(spKDArraySplitResultGetLeft(splitResult), splitMethod);
	treeNode->rightChild = buildTree(spKDArraySplitResultGetRight(splitResult), splitMethod);
	treeNode->data = NULL;
	// The left are right arrays are used, so we do not free them
	free(splitResult);
	return treeNode;
}
