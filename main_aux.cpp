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
#include "sp_util.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
}

static const int MAX_PATH_LENGTH = 100;

using namespace sp;


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

SPPoint **createImagesDatabase(SPConfig config, SP_DATABASE_CREATION_MSG *msg) {
	char *imagePath = NULL, *featuresPath = NULL;
	SPPoint **database = NULL;
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
	printf("Got here!\n");
	ImageProc ip = ImageProc(config);

	database = (SPPoint **) malloc(numOfImages * sizeof(*database));
	int *numberOfFeaturesPerImage = (int *) malloc (numOfImages * sizeof(int));
	imagePath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	featuresPath = (char *) malloc (MAX_PATH_LENGTH * sizeof(char));
	if (database == NULL || imagePath == NULL || featuresPath == NULL) {
		printf("Freeing 1 \n");
		free(database);
		free(imagePath);
		free(featuresPath);
		free(numberOfFeaturesPerImage);
		*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
		return NULL;
	}
	int numOfFeaturesExtracted;
	for (int imageIndex = 0; imageIndex < numOfImages; imageIndex++) {
		if (spConfigGetImagePath(imagePath, config, imageIndex) != SP_CONFIG_SUCCESS ||
				spConfigGetImageFeaturesPath(featuresPath, config, imageIndex) != SP_CONFIG_SUCCESS) {
			printf("Freeing 2 \n");
			for (int j = 0; j < imageIndex; j++) {
				freePointsArray(database[j], numberOfFeaturesPerImage[j]);
			}
			free(database);
			free(imagePath);
			free(featuresPath);
			free(numberOfFeaturesPerImage);
			*msg = SP_DATABASE_CREATION_ALLOC_FAIL;
			return NULL;
		}
		SPPoint *features = ip.getImageFeatures(imagePath, imageIndex, &numOfFeaturesExtracted);
		SP_DATABASE_CREATION_MSG creationMSG = writeFeatures(featuresPath, features, numOfFeaturesExtracted);
		if (creationMSG != SP_DATABASE_CREATION_SUCCESS) {
			printf("Freeing 3 \n");
			for (int j = 0; j < imageIndex; j++) {
				freePointsArray(database[j], numberOfFeaturesPerImage[j]);
			}
			free(database);
			free(imagePath);
			free(featuresPath);
			free(numberOfFeaturesPerImage);
			*msg = creationMSG;
			return NULL;
		}
		database[imageIndex] = features;
		numberOfFeaturesPerImage[imageIndex] = numOfFeaturesExtracted;
	}

	for (int j = 0; j < numOfImages; j++) {
		freePointsArray(database[j], numberOfFeaturesPerImage[j]);
	}
	free(database);
	free(imagePath);
	free(featuresPath);
	free(numberOfFeaturesPerImage);
	*msg = SP_DATABASE_CREATION_SUCCESS;
	return database;
}
