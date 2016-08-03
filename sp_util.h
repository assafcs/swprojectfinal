/*
 * sp_util.h
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#ifndef SP_UTIL_
#define SP_UTIL_

#include <stdlib.h>

char **spUtilStrSplit(char* str, const char delim, int *stringsCount);

char *spUtilStrJoin(char **strs, int stringsCount, const char delim);

void freeStringsArray(char **strings, int count);

#endif /* SP_UTIL_ */
