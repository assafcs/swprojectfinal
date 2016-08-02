/*
 * main_aux.cpp
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPImageProc.h"
#include "main_aux.h"
extern "C" {
#include "SPKDArray.h"
#include "sp_util.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

static const int MAX_PATH_LENGTH = 100;

using namespace sp;

//
//SPPoint *loadImageFeatures(const char *featuresPath, SP_DATABASE_CREATION_MSG *msg) {
//	featuresPath = NULL;
//	*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
//	return NULL;
//}
//
//SPPoint **loadImagesDatabase(SPConfig config, int numOfImages, SP_DATABASE_CREATION_MSG *msg) {
//	SP_CONFIG_MSG resultMSG;
//
//	SPPoint **database = (SPPoint **) malloc(numOfImages * sizeof(*database));
//	if (database == NULL) {
//		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
//		return NULL;
//	}
//
//	char *featuresPath = (char *) malloc(MAX_PATH_LENGTH * sizeof(char));
//	if (featuresPath == NULL) {
//		free(database);
//		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
//		return NULL;
//	}
//
//	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
//
//		if (spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
//			free(featuresPath);
//			free(database);
//			*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
//			return NULL;
//		}
//	}
//
//}

SP_DATABASE_CREATION_MSG writeFeature(FILE* featureFile, SPPoint feature) {
	if (featureFile == NULL) {
		return SP_DATABASE_CREATION_INVALID_ARGUMENT;
	}
	int dim = spPointGetDimension(feature);
	char **pointCoordinates = (char **) malloc(dim * sizeof(*pointCoordinates));
	if (pointCoordinates == NULL) {
		return SP_DATABASE_CREATION_ALLOC_FAIL;
	}
	for (int i = 0; i < dim; i++) {
		double coordinate = spPointGetAxisCoor(feature, i);
		char *pointCoordinate = (char *) malloc(20 * sizeof(char));
		int numOfChars = sprintf(pointCoordinate, "%f", coordinate);
		if (pointCoordinate == NULL || numOfChars == 0 || numOfChars > 19) {
			for (int j = 0; j < i; j++) {
				free(pointCoordinates[i]);
			}
			free(pointCoordinates);
			return SP_DATABASE_CREATION_INVALID_ARGUMENT;
		}
		pointCoordinates[i] = pointCoordinate;
	}
	SP_DATABASE_CREATION_MSG returnMsg = SP_DATABASE_CREATION_SUCCESS;
	char *joinedCoordinates = spUtilStrJoin(pointCoordinates, dim, ' ');
	if (joinedCoordinates == NULL) {
		returnMsg = SP_DATABASE_CREATION_ALLOC_FAIL;
	} else {
		if (fputs(joinedCoordinates, featureFile) == EOF || fputc('\n', featureFile) == EOF) {
			returnMsg = SP_DATABASE_CREATION_WRITE_ERROR;
		}
	}
	free(joinedCoordinates);
	for (int i = 0; i < dim; i++) {
		free(pointCoordinates[i]);
	}
	free(pointCoordinates);
	return returnMsg;
}

SP_DATABASE_CREATION_MSG writeFeatures(char *filePath, SPPoint *features, int numOfFeatures) {
	FILE *featuresFile = fopen(filePath, "w");
	if (numOfFeatures == 0) {
		return SP_DATABASE_CREATION_INVALID_ARGUMENT;
	}
	char numOfFeaturesAsString[10];
	int numberOfChars = sprintf(numOfFeaturesAsString, "%d", numOfFeatures);
	if (numberOfChars == 0 || numberOfChars > 9) {
		return SP_DATABASE_CREATION_INVALID_ARGUMENT;
	}
	if (fputs(numOfFeaturesAsString, featuresFile) == EOF || fputc('\n', featuresFile) == EOF) {
		fclose(featuresFile);
		remove(filePath);
		return SP_DATABASE_CREATION_WRITE_ERROR;
	}

	SP_DATABASE_CREATION_MSG msg;
	for (int i = 0; i < numOfFeatures; i++) {
		SPPoint feature = features[i];
		msg = writeFeature(featuresFile, feature);
		if (msg != SP_DATABASE_CREATION_SUCCESS) {
			fclose(featuresFile);
			remove(filePath);
			return msg;
		}
	}
	fclose(featuresFile);
	return SP_DATABASE_CREATION_SUCCESS;
}

void destroyVariables(SPPoint *allFeatures, int totalFeaturesCount, char *imagePath, char *featuresPath) {
	for (int j = 0; j < totalFeaturesCount; j++) {
		spPointDestroy(allFeatures[j]);
	}
	free(allFeatures);
	free(imagePath);
	free(featuresPath);
}

SPPoint *extractAllFeatures(SPConfig config, int *numberOfFeatures, SP_DATABASE_CREATION_MSG *msg) {
	char *imagePath = NULL, *featuresPath = NULL;
	SPPoint *allFeatures = NULL;

	if (config == NULL) {
		*msg = SP_DATABASE_CREATION_INVALID_ARGUMENT;
		return NULL;
	}
	SP_CONFIG_MSG resultMSG;
	int numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	bool extractionMode = spConfigIsExtractionMode(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}
	if (extractionMode) {
		//return loadImagesDatabase(config, numOfImages, msg);
		return NULL;
	}
	ImageProc ip = ImageProc(config);

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	imagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || imagePath == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, imagePath, featuresPath);
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return NULL;
	}
	int numOfFeaturesExtracted;
	int totalFeaturesCount = 0;
	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImagePath(imagePath, config, imageIndex) != SP_CONFIG_SUCCESS ||
				spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
			return NULL;
		}
		SPPoint *features = ip.getImageFeatures(imagePath, imageIndex, &numOfFeaturesExtracted);
		if (features == NULL || numOfFeaturesExtracted <= 0) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			*msg = SP_DATABASE_CREATION_FEATURES_EXTRACTION_ERROR;
			return NULL;
		}

		totalFeaturesCount += numOfFeaturesExtracted;
		SP_DATABASE_CREATION_MSG creationMSG = writeFeatures(featuresPath, features, numOfFeaturesExtracted);

		if (creationMSG != SP_DATABASE_CREATION_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			freePointsArray(features, numOfFeaturesExtracted);
			*msg = creationMSG;
			return NULL;
		}

		allFeatures = (SPPoint *) realloc(allFeatures, totalFeaturesCount * sizeof(SPPoint));
		if (allFeatures == NULL) {
			destroyVariables(allFeatures, totalFeaturesCount, imagePath, featuresPath);
			freePointsArray(features, numOfFeaturesExtracted);
			*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (int i = 0; i < numOfFeaturesExtracted; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesExtracted + i] = features[i];
		}
	}

	free(imagePath);
	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_DATABASE_CREATION_SUCCESS;
	return allFeatures;
}

SPKDTreeNode createImagesSearchTree(SPConfig config, SP_DATABASE_CREATION_MSG *msg) {
	SPPoint *allFeatures = NULL;

	int totalFeaturesCount;
	allFeatures = extractAllFeatures(config, &totalFeaturesCount, msg);
	if (allFeatures == NULL || totalFeaturesCount <= 0 || *msg != SP_DATABASE_CREATION_SUCCESS) {
		free(allFeatures);
		return NULL;
	}

	SPKDArray kdArray = spKDArrayInit(allFeatures, totalFeaturesCount);
	if (kdArray == NULL) {
		free(allFeatures);
		*msg = SP_DATABASE_CREATION_FEATURES_EXTRACTION_ERROR;
		return NULL;
	}
	SP_CONFIG_MSG configMsg;
	SP_TREE_SPLIT_METHOD splitMethod = spConfigGetSplitMethod(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		spKDArrayDestroy(kdArray);
		free(allFeatures);
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}
	SPKDTreeNode tree = spKDTreeBuild(kdArray, splitMethod);
	if (tree == NULL) {
		spKDArrayDestroy(kdArray);
		free(allFeatures);
		*msg = SP_DATABASE_CREATION_FEATURES_EXTRACTION_ERROR;
		return NULL;
	}
	return tree;
}
