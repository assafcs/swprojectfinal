/*
 * SPKDTree.h
 *
 *  Created on: Jul 19, 2016
 *      Author: mataneilat
 */

#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include "SPKDArray.h"
#include "SPConfig.h"

typedef struct sp_kd_tree_node_t *SPKDTreeNode;

SPKDTreeNode spKDTreeBuild(SPKDArray kdArray, SP_TREE_SPLIT_METHOD splitMethod);

int spKDTreeNodeGetDimension(SPKDTreeNode treeNode);

double spKDTreeNodeGetMedianValue(SPKDTreeNode treeNode);

SPKDTreeNode spKDTreeNodeGetLeftChild(SPKDTreeNode treeNode);

SPKDTreeNode spKDTreeNodeGetRightChild(SPKDTreeNode treeNode);

SPPoint *spKDTreeNodeGetData(SPKDTreeNode treeNode);

void spKDTreeDestroy(SPKDTreeNode treeNode);

#endif /* SPKDTREE_H_ */
