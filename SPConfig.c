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

typedef enum sp_parameter_parse_msg_t {
	SP_PARAMETER_PARSE_INVALID_KEY,
	SP_PARAMETER_PARSE_INVALID_ENUM_VALUE,
	SP_PARAMETER_PARSE_INVALID_INTEGER_FORMAT,
	SP_PARAMETER_PARSE_INVALID_BOOL_FORMAT,
	SP_PARAMETER_PARSE_SUCCESS
} SP_PARAMETER_PARSE_MSG;


int intValue(const char *parameterAsString, bool* success);
SP_PARAMETER_PARSE_MSG parseParameter(SPConfig config, char *key, char *value,
		char *requiredFieldsBitMask, bool* usedValueAsString);
int setupConfigWithDefaultValues(SPConfig config);
SPConfig initConfig();
void addCharacterToWord(char c, char** word, int* wordSize, int* wordCapacity);
char *defaultStringValue(const char *val);

static const char IMAGES_DIRECTORY_BIT_MASK = 0x01;
static const char IMAGES_PREFIX_BIT_MASK = 0x02;
static const char IMAGES_SUFFIX_BIT_MASK = 0x04;
static const char NUM_IMAGES_BIT_MASK = 0x08;

struct sp_config_t {
	// Required Fields
	char *imagesDirectory;
	char *imagesPrefix;
	char *imagesSuffix;
	int numOfImages;

	// Optional Fields - Have default values
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
	FILE *configFileStream;
	KeyToValue *currentParameter;
	SP_PARAMETER_READ_MSG parameterReadMsg;
	SP_PARAMETER_PARSE_MSG parameterParseMsg;
	bool isDone = false, usedValueAsString = false;
	char requiredFieldsBitMask = 0x00;
	SPConfig config, returnValue;

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	configFileStream = fopen(filename, "r");
	if (configFileStream == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	config = initConfig();
	if (config == NULL) {
		fclose(configFileStream);
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	if (setupConfigWithDefaultValues(config) == 1) {
		fclose(configFileStream);
		spConfigDestroy(config);
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	returnValue = config;
	*msg = SP_CONFIG_SUCCESS;

	while (!isDone) {
		currentParameter = nextParameter(configFileStream, &parameterReadMsg, &isDone);
		switch (parameterReadMsg) {
		case SP_PARAMETER_READ_ALLOCATION_FAILED:
			returnValue = NULL;
			*msg = SP_CONFIG_ALLOC_FAIL;
			isDone = true;
			break;
		case SP_PARAMETER_READ_INVALID_FORMAT:
			returnValue = NULL;
			*msg = SP_CONFIG_INVALID_STRING;
			isDone = true;
			break;
		case SP_PARAMETER_READ_SUCCESS:
			parameterParseMsg = parseParameter(config, currentParameter->key, currentParameter->value,
					&requiredFieldsBitMask, &usedValueAsString);
			switch (parameterParseMsg) {
			case SP_PARAMETER_PARSE_INVALID_INTEGER_FORMAT:
				returnValue = NULL;
				*msg = SP_CONFIG_INVALID_INTEGER;
				isDone = true;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}

		if (currentParameter != NULL) {
			free(currentParameter->key);
			if (!usedValueAsString) {
				free(currentParameter->value);
			}
			free(currentParameter);
		}
	}

	if ((requiredFieldsBitMask & IMAGES_DIRECTORY_BIT_MASK) == 0x00) {
		*msg = SP_CONFIG_MISSING_DIR;
		returnValue = NULL;
	} else if ((requiredFieldsBitMask & IMAGES_PREFIX_BIT_MASK) == 0x00) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		returnValue = NULL;
	} else if ((requiredFieldsBitMask & IMAGES_SUFFIX_BIT_MASK) == 0x00) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		returnValue = NULL;
	} else if ((requiredFieldsBitMask & NUM_IMAGES_BIT_MASK) == 0x00) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		returnValue = NULL;
	}

	if (returnValue == NULL) {
		spConfigDestroy(config);
	}

	fclose(configFileStream);
	return returnValue;
}

SPConfig initConfig() {
	SPConfig config = (SPConfig) malloc(sizeof(*config));
	if (config == NULL) {
		return NULL;
	}
	config->imagesDirectory = NULL;
	config->imagesPrefix = NULL;
	config->imagesSuffix = NULL;
	config->PCAFilename = NULL;
	config->loggerFilename = NULL;
	return config;
}

// Return 1 if allocation error occurred.
int setupConfigWithDefaultValues(SPConfig config) {
	char *PCAFilename = defaultStringValue("pca.yml");
	char *loggerFilename = defaultStringValue("stdout");

	if (PCAFilename == NULL || loggerFilename == NULL) {
		return 1;
	}

	config->PCADimension = 20;
	config->PCAFilename = PCAFilename;
	config->numOfFeatures = 100;
	config->extractionMode = true;
	config->minimalGUI = false;
	config->numOfSimilarImages = 1;
	config->KNN = 1;
	config->splitMethod = TREE_SPLIT_METHOD_MAX_SPREAD;
	config->loggerLevel = 3;
	config->loggerFilename = loggerFilename;
	return 0;
}

char *defaultStringValue(const char *val) {
	char *defaultValue =  (char *) malloc(sizeof(char) * (strlen(val) + 1));
	if (defaultValue == NULL) {
		return NULL;
	}
	strcpy(defaultValue, val);
	return defaultValue;
}

SP_PARAMETER_PARSE_MSG parseParameter(SPConfig config, char *key, char *value,
		char *requiredFieldsBitMask, bool* usedValueAsString) {
	bool conversionSucceeded = false;
	int parsedInt;
	//bool parsedBool;

	*usedValueAsString = false;
	if (strcmp(key, "spImagesDirectory") == 0) {
		*requiredFieldsBitMask |= IMAGES_DIRECTORY_BIT_MASK;
		config->imagesDirectory = value;
		*usedValueAsString = true;
		return SP_PARAMETER_PARSE_SUCCESS;
	} else if (strcmp(key, "spImagesPrefix") == 0) {
		*requiredFieldsBitMask |= IMAGES_PREFIX_BIT_MASK;
		config->imagesPrefix = value;
		*usedValueAsString = true;
		return SP_PARAMETER_PARSE_SUCCESS;
	} else if (strcmp(key, "spImagesSuffix") == 0) {
		*requiredFieldsBitMask |= IMAGES_SUFFIX_BIT_MASK;
		config->imagesSuffix = value;
		*usedValueAsString = true;
		return SP_PARAMETER_PARSE_SUCCESS;
	} else if (strcmp(key, "spNumOfImages") == 0) {
		*requiredFieldsBitMask |= NUM_IMAGES_BIT_MASK;
		parsedInt = intValue(value, &conversionSucceeded);
		if (conversionSucceeded && parsedInt >= 0) {
			config->numOfImages = parsedInt;
		} else {
			return SP_PARAMETER_PARSE_INVALID_INTEGER_FORMAT;
		}
	} else if (strcmp(key, "spPCADimension") == 0) {
		parsedInt = intValue(value, &conversionSucceeded);
		if (conversionSucceeded && parsedInt >= 10 && parsedInt <= 28) {
			config->PCADimension = parsedInt;
		} else {
			return SP_PARAMETER_PARSE_INVALID_INTEGER_FORMAT;
		}
	} else if (strcmp(key, "spPCAFilename") == 0) {
		config->PCAFilename = value;
		*usedValueAsString = true;
	} else if (strcmp(key, "spNumOfFeatures") == 0) {
		parsedInt = intValue(value, &conversionSucceeded);
		if (conversionSucceeded && parsedInt >= 0) {
			config->numOfFeatures = parsedInt;
		} else {
			return SP_PARAMETER_PARSE_INVALID_INTEGER_FORMAT;
		}
	}

	return SP_PARAMETER_PARSE_SUCCESS;
	// TODO: FINISH ALL PARAMETERS PARSE
}

int intValue(const char *parameterAsString, bool* success) {
	int val;
	if (strcmp(parameterAsString, "0") == 0) {
		*success = true;
		return 0;
	}
	val = atoi(parameterAsString);
	*success = (val != 0);
	return val;
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

void spConfigDestroy(SPConfig config) {
	if (config == NULL) {
		return;
	}
	free(config->imagesDirectory);
	free(config->imagesPrefix);
	free(config->imagesSuffix);
	free(config->PCAFilename);
	free(config->loggerFilename);
	free(config);
}

/*** Access Methods ***/

char *spConfigImagesDirectory(const SPConfig config, SP_CONFIG_MSG *msg) {
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->imagesDirectory;
}

char *spConfigImagesPrefix(const SPConfig config, SP_CONFIG_MSG *msg) {
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->imagesPrefix;
}

