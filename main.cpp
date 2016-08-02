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
#include "SPLogger.h"
#include "SPKDTree.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

int main() {

	//SP_LOGGER_MSG loggerMSG = spLoggerCreate("stdout", SP_LOGGER_ERROR_LEVEL);

	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate("inter.config", &resultMSG);

	SP_DATABASE_CREATION_MSG msg;

	SPKDTreeNode searchTree = createImagesSearchTree(config, &msg);

	int resultsCount;
	const char *queryPath = "./internet_images/Figure0.png";
	int *similarImages = findSimilarImagesIndices(config, queryPath, searchTree, &resultsCount);
	printf("Results: %d \n ", resultsCount);
	for (int i = 0; i < resultsCount; i++) {
		printf("%d ", similarImages[i]);
	}
	printf("\n");
	return 0;
}


