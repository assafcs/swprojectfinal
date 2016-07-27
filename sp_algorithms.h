/*
 * algorithms.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef SP_ALGORITHMS_H_
#define SP_ALGORITHMS_H_

#include <stdlib.h>
#include "SPBPriorityQueue.h"
#include "SPKDTree.h"
#include "SPPoint.h"

void spKNearestNeighbours(SPKDTreeNode tree, SPBPQueue queue, SPPoint point);

#endif /* SP_ALGORITHMS_H_ */
