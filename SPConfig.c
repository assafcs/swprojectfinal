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
#include <string.h>

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

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	FILE *configFile;
	//KeyToValue currentParameter;
	//SP_CONFIG_MSG parameterReadMsg;
	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	configFile = fopen(filename, "r");
	if (configFile == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}

//	while (1) {
//
//	}
	return NULL;
}

KeyToValue *nextParameter(FILE *stream, SP_PARAMETER_READ_MSG *msg, bool* reachedEnd) {
	KeyToValue *readParameter = NULL;
	char ch;
	int keyCapacity = 16, valueCapacity = 16;
	int *currentCapacity = &keyCapacity;
	int keySize = 0, valueSize = 0;
	int *currentSize = &keySize;
	bool invalid = false, lineBreak = false, commentLine = false;
	char *key, *value, *outputKey, *outputValue;
	char **current;

	key = (char *) malloc(sizeof(char) * keyCapacity);
	if (key == NULL) {
		*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
		return NULL;
	}

	value = (char *) malloc(sizeof(char) * valueCapacity);
	if (key == NULL) {
		free(key);
		*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
		return NULL;
	}

	current = &key;

	while ((ch = getc(stream)) != EOF) {
		if (ch == '\n') {
			if (current == &value && valueSize > 0) {
				addCharacterToWord('\0', current, currentSize, currentCapacity);
				current = NULL;
				currentSize = currentCapacity = NULL;
			}
			lineBreak = true;
			break;
		}

		if (invalid || commentLine) {
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
			} else if (ch == '#' && current == &key && keySize == 0) {
				commentLine = true;
				continue;
			}
			addCharacterToWord(ch, current, currentSize, currentCapacity);
		}
	}

	if (!lineBreak) {
		// In case the loop exited because of EOF - complete the value if needed
		if (current == &value && valueSize > 0) {
			addCharacterToWord('\0', current, currentSize, currentCapacity);
			current = NULL;
			currentSize = currentCapacity = NULL;
		}
		// Informs the the stream reached EOF
		*reachedEnd = true;
	}

	*msg = SP_PARAMETER_READ_INVALID_FORMAT;

	if (!invalid) {
		if (commentLine) {
			*msg = SP_PARAMETER_READ_COMMENT_LINE;
		} else if (keySize == 0 && valueSize == 0) {
			*msg = SP_PARAMETER_READ_EMPTY_LINE;
		} else if (current == NULL && keySize > 0 && valueSize > 0) {
			// Success reading key and value
			outputKey = (char *) malloc(keySize * sizeof(char));
			outputValue = (char *) malloc(valueSize * sizeof(char));
			readParameter = (KeyToValue *) malloc(sizeof(KeyToValue));
			if (outputKey == NULL || outputValue == NULL || readParameter == NULL) {
				free(outputKey);
				free(outputValue);
				free(readParameter);
				readParameter = NULL;
				*msg = SP_PARAMETER_READ_ALLOCATION_FAILED;
			} else {
				strcpy(outputKey, key);
				strcpy(outputValue, value);

				readParameter->key = outputKey;
				readParameter->value = outputValue;

				*msg = SP_PARAMETER_READ_SUCCESS;
			}
		}
	}

	free(key);
	free(value);

	return readParameter;
}

void addCharacterToWord(char c, char** word, int* wordSize, int* wordCapacity) {
	if ((*wordSize) >= (*wordCapacity)) {
		(*wordCapacity) *= 2;
		(*word) = (char*) realloc((*word), (*wordCapacity) * sizeof(char));
	}
	(*word)[*wordSize] = c;
	(*wordSize)++;
}

