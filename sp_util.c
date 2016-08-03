/*
 * sp_util.c
 *
 *  Created on: Jul 27, 2016
 *      Author: mataneilat
 */

#include "sp_util.h"
#include <stdio.h>
#include <string.h>

static const int TOKEN_MAX_LEN = 100;

char **spUtilStrSplit(char* str, const char delim, int *stringsCount) {

	int i, j, tokenIdx, resultIdx, count = 0, strLen = strlen(str);
    char **result, *token, *tokenDup = NULL;

    if (str  == NULL || strLen == 0) {
    	return NULL;
    }

    for (j = 0; j < strLen; j++) {
    	if (str[j] == delim) {
        	count++;
    	}
    }

    // The string after the last delimiter
    count++;

    result = (char **) malloc(count * sizeof(char *));
    if (result == NULL) {
    	return NULL;
	}

    token = (char *) malloc(TOKEN_MAX_LEN * sizeof(char));
    if (token == NULL) {
    	free(result);
    	return NULL;
    }

    tokenIdx = 0;
    resultIdx = 0;
    for (i = 0; i < strLen; i++) {
    	if (str[i] == delim) {
    		token[tokenIdx] = '\0';
    		tokenDup = strdup(token);
    		if (tokenDup == NULL) {
    			for (j = 0; j < i; j++) {
    				free(result[j]);
    		    }
    		    free(result);
    		    return NULL;
    		}
    		result[resultIdx] = tokenDup;
    		resultIdx++;
    		tokenIdx = 0;
    	} else {
    		token[tokenIdx] = str[i];
    		tokenIdx++;
    	}
    }

    token[tokenIdx] = '\0';
    tokenDup = strdup(token);
    if (tokenDup == NULL) {
    	for (j = 0; j < i; j++) {
    		free(result[j]);
    	}
    	free(result);
        return NULL;
    }
    result[resultIdx] = tokenDup;

    free(token);

    *stringsCount = count;
    return result;
}


char *spUtilStrJoin(char **strs, int stringsCount, const char delim) {
	int i, j, resultIdx = 0, totalLength = 0;
	char *result;
	char *currentStr = NULL;
	if (strs == NULL) {
		return NULL;
	}

	for (i = 0; i < stringsCount; i++) {
		totalLength += (strlen(strs[i]) + 1);
	}

	result = (char *) malloc(totalLength * sizeof(char));
	if (result == NULL) {
		return NULL;
	}

	for (i = 0; i < stringsCount; i++) {
		currentStr = strs[i];
		for (j = 0; j < (int)strlen(currentStr) ; j++) {
			result[resultIdx] = currentStr[j];
			resultIdx++;
		}
		result[resultIdx] = delim;
		resultIdx++;
	}

	result[totalLength - 1] = '\0';
	return result;
}

void freeStringsArray(char **strings, int count) {
	if (strings == NULL) {
		return;
	}
	int i;
	for (i = 0; i < count; i++) {
		free(strings[i]);
	}
	free(strings);
}


