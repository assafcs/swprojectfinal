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

static bool invalidParameterRead(FILE *stream);
static bool properParametersRead(FILE *stream, const char *expectedKey, const char *expectedValue);

static bool spConfigTest() {
	FILE *f = fopen("/Users/mataneilat/Documents/workspace/swprojectfinal/test_config.txt", "r");

	ASSERT(invalidParameterRead(f));
	ASSERT(properParametersRead(f, "ABS", "CDS"));
	ASSERT(properParametersRead(f, "M", "2"));
	ASSERT(invalidParameterRead(f));
	ASSERT(invalidParameterRead(f));
	ASSERT(invalidParameterRead(f));
	ASSERT(properParametersRead(f, "ll", "123456789012345678901234567890"));
	ASSERT(invalidParameterRead(f));
	ASSERT(invalidParameterRead(f));
	ASSERT(invalidParameterRead(f));

	fclose(f);
	return true;
}

static bool invalidParameterRead(FILE *stream) {
	KeyToValue result;
	SP_CONFIG_MSG resultMSG = nextParameter(stream, &result);
	ASSERT_SAME(resultMSG, SP_CONFIG_INVALID_ARGUMENT);
	return true;
}

static bool properParametersRead(FILE *stream, const char *expectedKey, const char *expectedValue) {
	KeyToValue result;
	SP_CONFIG_MSG resultMSG = nextParameter(stream, &result);
	ASSERT_SAME(resultMSG, SP_CONFIG_SUCCESS);
	ASSERT_SAME(strcmp(result.key, expectedKey), 0);
	ASSERT_SAME(strcmp(result.value, expectedValue), 0);
	return true;
}

int main() {
	RUN_TEST(spConfigTest);
}
