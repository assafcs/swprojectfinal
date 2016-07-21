/*
 * sp_config_unit_test.c
 *
 *  Created on: Jul 20, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include "unit_test_util.h"
#include "../SPConfig.h"

static bool invalidParameterRead(FILE *stream, SP_PARAMETER_READ_MSG expectedMsg, bool isEOFExpected);
static bool properParametersRead(FILE *stream, const char *expectedKey, const char *expectedValue, bool isEOFExpected);
static bool configCreateError(const char *filename, SP_CONFIG_MSG expectedMsg);

static bool spConfigNullFilenameTest() {
	ASSERT(configCreateError(NULL, SP_CONFIG_INVALID_ARGUMENT));
	return true;
}

static bool spConfigMissingFileTest() {
	ASSERT(configCreateError("missing", SP_CONFIG_CANNOT_OPEN_FILE));
	return true;
}

static bool spConfigTest() {
	FILE *f = fopen("/Users/mataneilat/Documents/workspace/swprojectfinal/test_config.txt", "r");

	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_EMPTY_LINE, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_COMMENT_LINE, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_COMMENT_LINE, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(f, "ABS", "CDS", false));
	ASSERT(properParametersRead(f, "M", "2", false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(f, "ll", "123456789012345678901234567890", false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(invalidParameterRead(f, SP_PARAMETER_READ_INVALID_FORMAT, false));
	ASSERT(properParametersRead(f, "d", "3", true));

	fclose(f);
	return true;
}

/*** Help Assert Methods ***/

static bool configCreateError(const char *filename, SP_CONFIG_MSG expectedMsg) {
	SP_CONFIG_MSG returnMsg;
	SPConfig config = spConfigCreate(filename, &returnMsg);
	ASSERT_NULL(config);
	ASSERT_SAME(returnMsg, expectedMsg);
	return true;
}

static bool invalidParameterRead(FILE *stream, SP_PARAMETER_READ_MSG expectedMsg, bool isEOFExpected) {
	SP_PARAMETER_READ_MSG resultMsg;
	bool isEndOfFile = false;
	KeyToValue *result = nextParameter(stream, &resultMsg, &isEndOfFile);
	ASSERT_NULL(result);
	ASSERT_SAME(resultMsg, expectedMsg);
	ASSERT_SAME(isEndOfFile, isEOFExpected);
	return true;
}

static bool properParametersRead(FILE *stream, const char *expectedKey, const char *expectedValue, bool isEOFExpected) {
	SP_PARAMETER_READ_MSG resultMSG;
	bool isEndOfFile = false;
	KeyToValue *result = nextParameter(stream, &resultMSG, &isEndOfFile);
	ASSERT_SAME(resultMSG, SP_PARAMETER_READ_SUCCESS);
	ASSERT_SAME(strcmp(result->key, expectedKey), 0);
	ASSERT_SAME(strcmp(result->value, expectedValue), 0);
	ASSERT_SAME(isEndOfFile, isEOFExpected);
	return true;
}

int main() {
	RUN_TEST(spConfigTest);
	RUN_TEST(spConfigNullFilenameTest);
	RUN_TEST(spConfigMissingFileTest);

}
