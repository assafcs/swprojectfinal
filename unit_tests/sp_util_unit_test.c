/*
 * sp_util_unit_test.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "../sp_util.h"
#include "unit_test_util.h"

static bool spUtilSplitTest() {
	char *str = "This is a string";
	int numberOfStrings = 0;
	char **result = spUtilStrSplit(str, ' ', &numberOfStrings);
	ASSERT_NOT_NULL(result);
	ASSERT_SAME(numberOfStrings, 4);
	ASSERT_SAME(strcmp(result[0], "This"), 0);
	ASSERT_SAME(strcmp(result[1], "is"), 0);
	ASSERT_SAME(strcmp(result[2], "a"), 0);
	ASSERT_SAME(strcmp(result[3], "string"), 0);

	freeStringsArray(result, 4);
	return true;
}

static bool spUtilJoinTest() {
	int numberOfStrings = 5;
	char *joinedString;
	char **strs = (char **) malloc(numberOfStrings * sizeof(char *));
	strs[0] = "1";
	strs[1] = "Two";
	strs[2] = "3";
	strs[3] = "Four";
	strs[4] = "5";

	joinedString = spUtilStrJoin(strs, numberOfStrings, '^');

	ASSERT_NOT_NULL(joinedString);
	ASSERT_SAME(strcmp(joinedString, "1^Two^3^Four^5"), 0);

	free(joinedString);
	return true;
}

int main() {
	RUN_TEST(spUtilSplitTest);
	RUN_TEST(spUtilJoinTest);
}
