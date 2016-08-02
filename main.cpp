/*
 * main.cpp

 *
 *  Created on: Jul 24, 2016
 *      Author: mataneilat
 */

#include <stdio.h>
#include <stdlib.h>
#include "main_aux.h"
extern "C" {
#include "SPKDTree.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

int main() {

	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate("spconfig.config", &resultMSG);

	SP_DATABASE_CREATION_MSG msg;
	SPKDTreeNode searchTree = createImagesSearchTree(config, &msg);

	printf("Random Value: %d", spKDTreeNodeGetDimension(searchTree));
	return 0;
}


