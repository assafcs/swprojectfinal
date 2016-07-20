/*
 * SPConfig.c
 *
 *  Created on: Jul 19, 2016
 *      Author: mataneilat
 */

#include "SPConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

//typedef struct sp_key_to_value {
//	char *key;
//	char *value;
//} KeyToValue;

void addCharacterToWord(char c, char** word, int* wordSize, int* wordCapacity);

struct sp_config_t {
	char *imagesDirectory;
	char *imagesPrefix;
	char *imagesSuffix;
	int numOfImages;
	int PCADimension;
	char *PCAFilename;
	int numOfFeatures;
	int numOfSimilarImages;
	SP_TREE_SPLIT_METHOD splitMethod;
	int KNN;
	bool minimalGUI;
	int loggerLevel;
	char *loggerFilename;
	bool extractionMode;
};

//SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
//	FILE *configFile = open(filename, "r");
//
//}

SP_CONFIG_MSG nextParameter(FILE *stream, KeyToValue *outputParameter) {
	char ch;
	int keyCapacity = 16, valueCapacity = 16;
	int *currentCapacity = &keyCapacity;
	int keySize = 0, valueSize = 0;
	int *currentSize = &keySize;

	char *key = (char *) malloc(sizeof(char) * keyCapacity);
	if (key == NULL) {
		return SP_CONFIG_ALLOC_FAIL;
	}

	char *value = (char *) malloc(sizeof(char) * valueCapacity);
	if (key == NULL) {
		free(key);
		return SP_CONFIG_ALLOC_FAIL;
	}

	char **current = &key;

	bool invalid = false;

	while ((ch = getc(stream)) != EOF) {
		if (ch == '\n') {
			if (current == &value && valueSize > 0) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			break;
		}

		if (invalid) {
			// Wait for new line
			continue;
		}

		if (isspace(ch)) {
			if ((current == &key && keySize > 0) || (current == &value && valueSize > 0)) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			continue;
		}

		if (ch == '=') {
			if (current == &key && keySize > 0) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			if (current == NULL && keySize > 0 && valueSize == 0) {
				current = &value;
				currentSize = &valueSize;
				currentCapacity = &valueCapacity;
			} else {
				invalid = true;
				continue;
			}
		} else {
			if (current == NULL) {
				invalid = true;
				continue;
			}
			addCharacterToWord(ch, current, currentSize, currentCapacity);
		}
	}


	if (current == NULL && keySize > 0 && valueSize > 0) {
		// Success reading key and value
		outputParameter->key = key;
		outputParameter->value = value;
	} else {
		invalid = true;
	}

	if (invalid) {
		free(key);
		free(value);
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	return SP_CONFIG_SUCCESS;
}

void addCharacterToWord(char c, char** word, int* wordSize, int* wordCapacity) {
	if ((*wordSize) >= (*wordCapacity)) {
		(*wordCapacity) *= 2;
		(*word) = (char*) realloc((*word), (*wordCapacity) * sizeof(char));
	}
	(*word)[*wordSize] = c;
	(*wordSize)++;
}

