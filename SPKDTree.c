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

SPKDTreeNode buildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod, int previousSplitDimension);

SPKDTreeNode spKDTreeBuild(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod) {
	return buildTree(kdArray, splitMethod, -1);
}

SPKDTreeNode buildTree(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod, int previousSplitDimension) {
	int arraySize, splitDimension, maxDimension;
	SPKDTreeNode treeNode = NULL;
	SPKDArraySplitResult splitResult = NULL;
	SPPoint *data;
	if (kdArray == NULL) {
		return NULL;
	}
	arraySize = spKDArrayGetSize(kdArray);
	if (arraySize == 0) {
		return NULL;
	}
	treeNode = (SPKDTreeNode) malloc(sizeof(*treeNode));
	if (treeNode == NULL) {
		return NULL;
	}
	if (arraySize == 1) {
		data = (SPPoint *) malloc(sizeof(SPPoint));
		if (data == NULL) {
			free(treeNode);
			return NULL;
		}
		*data = spPointCopy(spKDArrayGetPointsArray(kdArray)[0]);

		treeNode->dim = -1;
		treeNode->medianVal = 0;
		treeNode->leftChild = NULL;
		treeNode->rightChild = NULL;
		treeNode->data = data;
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
	splitResult = spKDArraySplit(kdArray, splitDimension);
	if (splitResult == NULL) {
		free(treeNode);
		return NULL;
	}

	treeNode->dim = splitDimension;
	treeNode->medianVal = spKDArrayGetMedian(kdArray, splitDimension);
	treeNode->leftChild = buildTree(spKDArraySplitResultGetLeft(splitResult), splitMethod, splitDimension);
	treeNode->rightChild = buildTree(spKDArraySplitResultGetRight(splitResult), splitMethod, splitDimension);
	treeNode->data = NULL;
	// The left are right arrays are used, so we do not free them
	free(splitResult);
	return treeNode;
}


int spKDTreeNodeGetDimension(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return -1;
	return treeNode->dim;
}

double spKDTreeNodeGetMedianValue(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return 0;
	return treeNode->medianVal;
}

SPKDTreeNode spKDTreeNodeGetLeftChild(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->leftChild;
}

SPKDTreeNode spKDTreeNodeGetRightChild(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->rightChild;
}

SPPoint *spKDTreeNodeGetData(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return NULL;
	return treeNode->data;
}

void spKDTreeDestroy(SPKDTreeNode treeNode) {
	if (treeNode == NULL) return;
	if (treeNode->data != NULL) {
		spPointDestroy(*(treeNode->data));
	}
	spKDTreeDestroy(treeNode->leftChild);
	spKDTreeDestroy(treeNode->rightChild);
	free(treeNode);
}

