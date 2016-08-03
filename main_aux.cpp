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
#include "SPBPriorityQueue.h"
#include "sp_algorithms.h"
#include "SPKDArray.h"
#include "sp_util.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

static const int MAX_PATH_LENGTH = 100;

static const int MAX_FEATURE_COORDINATE_STRING_LEN = 20;

static const int MAX_NUM_OF_FEATURES_STRING_LEN = 10;

static const char FEATURE_COORDINATES_DELIM = ' ';

struct HitInfo {
   int index;
   int hits;
};

int cmpHitInfos(const void * a, const void * b) {
	HitInfo aInfo = *(HitInfo*)a;
	HitInfo bInfo = *(HitInfo*)b;
	int res = (bInfo.hits - aInfo.hits);
	if (res != 0) {
		return res;
	}
	return aInfo.index - bInfo.index;
}

using namespace sp;

void destroyVariables(SPPoint *allFeatures, int totalFeaturesCount, char *imagePath, char *featuresPath) {
	for (int j = 0; j < totalFeaturesCount; j++) {
		spPointDestroy(allFeatures[j]);
	}
	free(allFeatures);
	free(imagePath);
	free(featuresPath);
}

int loadNumberOfFeatures(FILE *featuresFile, SP_DATABASE_CREATION_MSG *msg) {
	char *numberOfFeaturesAsString = (char *) malloc((MAX_NUM_OF_FEATURES_STRING_LEN + 1) * sizeof(char));
	if (numberOfFeaturesAsString == NULL) {
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return -1;
	}
	if (fgets(numberOfFeaturesAsString, MAX_NUM_OF_FEATURES_STRING_LEN, featuresFile) == NULL) {
		free(numberOfFeaturesAsString);
		*msg = SP_DATABASE_CREATION_LOAD_ERROR;
		return -1;
	}
	int numberOfFeatures = atoi(numberOfFeaturesAsString);

	free(numberOfFeaturesAsString);
	*msg = (numberOfFeatures == 0) ? SP_DATABASE_CREATION_LOAD_ERROR : SP_DATABASE_CREATION_SUCCESS;
	return numberOfFeatures;
}

SPPoint loadFeature(FILE *featuresFile, int expectedDimension, int index, SP_DATABASE_CREATION_MSG *msg) {
	int maxFeatureStringLen = (MAX_FEATURE_COORDINATE_STRING_LEN + 1) * expectedDimension;
	char *featureCoordinatesString = (char *) malloc((maxFeatureStringLen + 1) * sizeof(char));
	if (featureCoordinatesString == NULL) {
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return NULL;
	}
	if (fgets(featureCoordinatesString, maxFeatureStringLen, featuresFile) == NULL) {
		free(featureCoordinatesString);
		*msg = SP_DATABASE_CREATION_LOAD_ERROR;
		return NULL;
	}

	int numberOfCoordinates;
	char **splitResult = spUtilStrSplit(featureCoordinatesString, FEATURE_COORDINATES_DELIM, &numberOfCoordinates);
	// The feature coordinates string is no longer needed..
	free(featureCoordinatesString);

	if (splitResult == NULL || numberOfCoordinates != expectedDimension) {
		freeStringsArray(splitResult, numberOfCoordinates);
		*msg = SP_DATABASE_CREATION_LOAD_ERROR;
		return NULL;
	}

	double *data = (double *) malloc(numberOfCoordinates * sizeof(double));
	for (int i = 0; i < numberOfCoordinates; i++) {
		double featureCoordinate = atof(splitResult[i]);
		if (featureCoordinate == 0 && strcmp(splitResult[i], "0") != 0) {
			free(data);
			freeStringsArray(splitResult, numberOfCoordinates);
			*msg = SP_DATABASE_CREATION_LOAD_ERROR;
			return NULL;
		}
		data[i] = featureCoordinate;
	}
	SPPoint feature = spPointCreate(data, numberOfCoordinates, index);
	free(data);
	freeStringsArray(splitResult, numberOfCoordinates);
	*msg = SP_DATABASE_CREATION_SUCCESS;
	return feature;
}

SPPoint *loadFeatures(const char *filePath, int index, int expectedFeatureDimension, int *numOfFeaturesLoaded,
		SP_DATABASE_CREATION_MSG *msg) {
	FILE *featuresFile = fopen(filePath, "r");
	if (featuresFile == NULL) {
		*msg = SP_DATABASE_CREATION_FEATURE_FILE_MISSING;
		return NULL;
	}
	int numberOfFeatures = loadNumberOfFeatures(featuresFile, msg);
	if (*msg != SP_DATABASE_CREATION_SUCCESS) {
		fclose(featuresFile);
		return NULL;
	}

	SPPoint *features = (SPPoint *) malloc(numberOfFeatures * sizeof(SPPoint));
	if (features == NULL) {
		fclose(featuresFile);
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return NULL;
	}
	for (int i = 0; i < numberOfFeatures; i++) {
		SPPoint feature = loadFeature(featuresFile, expectedFeatureDimension, index, msg);
		if (*msg != SP_DATABASE_CREATION_SUCCESS) {
			destroyVariables(features, i, NULL, NULL);
			fclose(featuresFile);
			return NULL;
		}
		features[i] = feature;
	}

	fclose(featuresFile);
	*msg = SP_DATABASE_CREATION_SUCCESS;
	*numOfFeaturesLoaded = numberOfFeatures;
	return features;

}

SPPoint *loadAllFeatures(SPConfig config, int *numberOfFeatures, SP_DATABASE_CREATION_MSG *msg) {
	char *featuresPath = NULL;
	SPPoint *allFeatures = NULL;

	SP_CONFIG_MSG resultMSG;
	int numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	int expectedDimension = spConfigGetPCADim(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}

	allFeatures = (SPPoint *) malloc(1 * sizeof(SPPoint));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (allFeatures == NULL || featuresPath == NULL) {
		destroyVariables(allFeatures, 0, NULL, featuresPath);
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return NULL;
	}

	int numOfFeaturesLoaded;
	int totalFeaturesCount = 0;

	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
			return NULL;
		}
		SPPoint *features = loadFeatures(featuresPath, imageIndex, expectedDimension, &numOfFeaturesLoaded, msg);
		if (*msg != SP_DATABASE_CREATION_SUCCESS) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			return NULL;
		}

		totalFeaturesCount += numOfFeaturesLoaded;

		allFeatures = (SPPoint *) realloc(allFeatures, totalFeaturesCount * sizeof(SPPoint));
		if (allFeatures == NULL) {
			destroyVariables(allFeatures, totalFeaturesCount, NULL, featuresPath);
			freePointsArray(features, numOfFeaturesLoaded);
			*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
			return NULL;
		}

		for (int i = 0; i < numOfFeaturesLoaded; i++) {
			allFeatures[totalFeaturesCount - numOfFeaturesLoaded + i] = features[i];
		}
	}

	free(featuresPath);
	*numberOfFeatures = totalFeaturesCount;
	*msg = SP_DATABASE_CREATION_SUCCESS;
	return allFeatures;
}

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
		char *pointCoordinate = (char *) malloc(MAX_FEATURE_COORDINATE_STRING_LEN * sizeof(char));
		int numOfChars = sprintf(pointCoordinate, "%f", coordinate);
		if (pointCoordinate == NULL || numOfChars == 0 || numOfChars > MAX_FEATURE_COORDINATE_STRING_LEN - 1) {
			for (int j = 0; j < i; j++) {
				free(pointCoordinates[i]);
			}
			free(pointCoordinates);
			return SP_DATABASE_CREATION_INVALID_ARGUMENT;
		}
		pointCoordinates[i] = pointCoordinate;
	}
	SP_DATABASE_CREATION_MSG returnMsg = SP_DATABASE_CREATION_SUCCESS;
	char *joinedCoordinates = spUtilStrJoin(pointCoordinates, dim, FEATURE_COORDINATES_DELIM);
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
	if (numOfFeatures == 0) {
		return SP_DATABASE_CREATION_INVALID_ARGUMENT;
	}
	char numOfFeaturesAsString[MAX_NUM_OF_FEATURES_STRING_LEN];
	int numberOfChars = sprintf(numOfFeaturesAsString, "%d", numOfFeatures);
	if (numberOfChars == 0 || numberOfChars > MAX_NUM_OF_FEATURES_STRING_LEN - 1) {
		return SP_DATABASE_CREATION_INVALID_ARGUMENT;
	}
	FILE *featuresFile = fopen(filePath, "w");
	if (featuresFile == NULL) {
		return SP_DATABASE_CREATION_WRITE_ERROR;
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

SPPoint *extractAllFeatures(SPConfig config, int *numberOfFeatures, SP_DATABASE_CREATION_MSG *msg) {
	char *imagePath = NULL, *featuresPath = NULL;
	SPPoint *allFeatures = NULL;

	SP_CONFIG_MSG resultMSG;
	int numOfImages = spConfigGetNumOfImages(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
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
			*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
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

SPPoint *getAllFeatures(SPConfig config, int *numberOfFeatures, SP_DATABASE_CREATION_MSG *msg) {

	if (config == NULL) {
		*msg = SP_DATABASE_CREATION_INVALID_ARGUMENT;
		return NULL;
	}
	SP_CONFIG_MSG resultMSG;
	bool extractionMode = spConfigIsExtractionMode(config, &resultMSG);
	if (resultMSG != SP_CONFIG_SUCCESS) {
		*msg = SP_DATABASE_CREATION_CONFIG_ERROR;
		return NULL;
	}
	return extractionMode ? extractAllFeatures(config, numberOfFeatures, msg) : loadAllFeatures(config, numberOfFeatures, msg);
}

SPKDTreeNode createImagesSearchTree(const SPConfig config, SP_DATABASE_CREATION_MSG *msg) {
	SPPoint *allFeatures = NULL;

	int totalFeaturesCount;
	allFeatures = getAllFeatures(config, &totalFeaturesCount, msg);
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

int *findSimilarImagesIndices(const SPConfig config, const char *queryImagePath, const SPKDTreeNode searchTree, int *resultsCount) {
	if (config == NULL || searchTree == NULL || resultsCount == NULL) {
		return NULL;
	}
	SP_CONFIG_MSG configMsg;
	int KNN = spConfigGetKNN(config, &configMsg);

	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}
	int numOfImages = spConfigGetNumOfImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}
	int similarImages = spConfigGetNumOfSimilarImages(config, &configMsg);
	if (configMsg != SP_CONFIG_SUCCESS) {
		return NULL;
	}

	HitInfo* hitInfos = (HitInfo*) malloc(numOfImages * sizeof(HitInfo));
	if (hitInfos == NULL) {
		return NULL;
	}

	for (int i = 0; i < numOfImages; i++) {
		struct HitInfo info = {i, 0};
		hitInfos[i] = info;
	}

	int numOfFeaturesExtracted;
	ImageProc ip = ImageProc(config);

	SPPoint *features = ip.getImageFeatures(queryImagePath, 0, &numOfFeaturesExtracted);

	if (features == NULL || numOfFeaturesExtracted <= 0) {
		free(hitInfos);
		return NULL;
	}

	SPBPQueue queue = spBPQueueCreate(KNN);
	for (int i = 0; i < numOfFeaturesExtracted; i++) {
		SPPoint feature = features[i];
		spKNearestNeighbours(searchTree, queue, feature);
		for (int j = 0; j < spBPQueueSize(queue); j++) {
			SPListElement listElement = spBPQueuePeek(queue);
			int index = spListElementGetIndex(listElement);
			hitInfos[index].hits++;
			spBPQueueDequeue(queue);
			spListElementDestroy(listElement);
		}
		spBPQueueClear(queue);
	}
	// No need for the queue anymore
	spBPQueueDestroy(queue);

	qsort(hitInfos, numOfImages, sizeof(HitInfo), cmpHitInfos);

	int* resValue = (int *) malloc(similarImages * sizeof(int));

	if (resValue == NULL) {
		free(hitInfos);
		return NULL;
	}

	for (int i = 0; i < similarImages; i++) {
		resValue[i] = hitInfos[i].index;
	}

	free(hitInfos);
	*resultsCount = similarImages;
	return resValue;
}
