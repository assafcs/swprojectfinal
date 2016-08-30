/*
 * main.cpp

 *
 *  Created on: Jul 24, 2016
 *      Author: mataneilat
 */

#include <cstdio>
#include <cstdlib>
#include <string.h>
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
#define QUERY_IMAGE_INPUT "Please enter an image path:\n"
#define EXIT_MESSAGE "Exiting…\n"

#define SP_LOGGER_CANNOT_OPEN_FILE_TEXT "The logger output file cannot be opened\n"

int main(int argc, char *argv[]) {

	// Input validation and in case of no config, default config file setting
	char filename[LINE_MAX_SIZE];
	if (argc > 3 || argc == 2 || (argc == 3 && argv[1] != "-c")) {
		printf(INVALID_COMMAND_LINE_TEXT);
		return 1;
	} else if (argc == 1) {
		filename = DEFAULT_CONFIG_FILENAME;
	} else {
		filename = argv[2];
	}

	// Creating config file and exiting on failure.
	// Error messages handling by SPConfig.c
	SP_CONFIG_MSG resultMSG;
	SPConfig config = spConfigCreate(filename, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS){
		return 1;
	}

	// Creating logger
	SP_LOGGER_MSG loggerMSG = spLoggerCreate(spConfigGetLoggerFilename(config), spConfigGetLoggerLevel(config));
	if (loggerMSG == SP_LOGGER_CANNOT_OPEN_FILE){
		printf(SP_LOGGER_CANNOT_OPEN_FILE_TEXT);
		return 1;
	}

	SP_DATABASE_CREATION_MSG msg;

	SPKDTreeNode searchTree = createImagesSearchTree(config, &msg);

	char imageQueryPath[LINE_MAX_SIZE];
	int resultsCount;
	while (true){
		printf(QUERY_IMAGE_INPUT);
		scanf("%s", imageQueryPath);
		if (strcmp(imageQueryPath, "<>") == 0) {
			break;
		} else {
			int *similarImages = findSimilarImagesIndices(config, imageQueryPath, searchTree, &resultsCount);
			printf("Results: %d \n ", resultsCount);
			for (int i = 0; i < resultsCount; i++) {
				printf("%d ", similarImages[i]);
			}
		}

	printf(EXIT_MESSAGE);
	return 0;
}


