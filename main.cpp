/*
 * main.cpp

 *
 *  Created on: Jul 24, 2016
 *      Author: mataneilat
 */

#include <cstdio>
#include <cstdlib>
#include "main_aux.h"
#include "SPImageProc.h"
extern "C" {
#include "SPLogger.h"
#include "SPKDTree.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

#define LINE_MAX_SIZE 1024

int main(int argc, char *argv[]) {

	char filename[LINE_MAX_SIZE];
	if (argc > 3 || argc == 2 || (argc == 3 && argv[1] != "-c")) {
		printf(INVALID_COMMAND_LINE_TEXT);
		return 1;
	} else if (argc == 1) {
		filename = DEFAULT_CONFIG_FILENAME;
	} else {
		filename = argv[2];
	}

	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate(filename, &resultMSG);
	if (resultMSG == SP_CONFIG_SUCCESS){
		SP_LOGGER_MSG loggerMSG = spLoggerCreate(spConfigGetLoggerFilename(config), spConfigGetLoggerLevel(config));
	} else {
		return 1;
	}

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


