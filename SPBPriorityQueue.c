/*
 * SBPPriorityQueue.c
 *
 *  Created on: Jun 3, 2016
 *      Author: mataneilat
 */


#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/*** Method declaration ***/

SPBPQueue spBPQueueInnerCreate(SPList innerList, int maxSize);
SPListElement spBPQueueFirstElement(SPBPQueue source);
SPListElement spBPQueueLastElement(SPBPQueue source);

struct sp_bp_queue_t {
	SPList innerList;
	int maxSize;
};

SPBPQueue spBPQueueCreate(int maxSize) {
	if (maxSize <= 0) {
		return NULL;
	}
	SPList newList = spListCreate();
	if (newList == NULL) {
		return NULL;
	}
	SPBPQueue createdQueue = spBPQueueInnerCreate(newList, maxSize);
	if (createdQueue == NULL) {
		spListDestroy(newList);
		return NULL;
	}
	return createdQueue;
}

SPBPQueue spBPQueueInnerCreate(SPList innerList, int maxSize) {
	SPBPQueue createdQueue = (SPBPQueue) malloc(sizeof(*createdQueue));
	if (createdQueue == NULL) {
		return NULL;
	}
	createdQueue->innerList = innerList;
	createdQueue->maxSize = maxSize;
	return createdQueue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source) {
	if (source == NULL) {
		return NULL;
	}
	SPList listCopy = spListCopy(source->innerList);
	if (listCopy == NULL) {
		return NULL;
	}
	return spBPQueueInnerCreate(listCopy, source->maxSize);
}

void spBPQueueDestroy(SPBPQueue source) {
	if (source == NULL) {
		return;
	}
	spListDestroy(source->innerList);
	free(source);
}

void spBPQueueClear(SPBPQueue source) {
	if (source == NULL) {
		return;
	}
	spListClear(source->innerList);
}

int spBPQueueSize(SPBPQueue source) {
	return source == NULL ? -1 : spListGetSize(source->innerList);
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	return source == NULL ? -1 : source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element) {
	if (source == NULL || element == NULL) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	bool wasFull = spBPQueueIsFull(source);

	SPList list = source->innerList;

	bool wasInserted = false;
	// Advance iterator to the proper element's place
	SP_LIST_FOREACH(SPListElement, currentElement, list) {

		if (spListElementCompare(element, currentElement) < 0) {

			// Modifying while iterating - bad practice but is more efficient here.
			SP_LIST_MSG returnMSG = spListInsertBeforeCurrent(list, element);

			switch (returnMSG) {
			case SP_LIST_OUT_OF_MEMORY:
				return SP_BPQUEUE_OUT_OF_MEMORY;
			default:
				break;
			}
			wasInserted = true;
			break;
		}
	}

	if (wasInserted) {
		// Remove the last element if needed
		if (wasFull) {
			spListGetLast(list);
			spListRemoveCurrent(list);
		}
		return SP_BPQUEUE_SUCCESS;
	} else {
		if (spBPQueueIsFull(source)) {
			return SP_BPQUEUE_FULL;
		} else {
			// The item is largest than everything, insert last
			SP_LIST_MSG returnMSG = spListInsertLast(list, element);
			switch (returnMSG) {
			case SP_LIST_OUT_OF_MEMORY:
				return SP_BPQUEUE_OUT_OF_MEMORY;
			default:
				return SP_BPQUEUE_SUCCESS;
			}
		}
	}
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	if (source == NULL) {
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	if (spBPQueueIsEmpty(source)) {
		return SP_BPQUEUE_EMPTY;
	}

	spListGetFirst(source->innerList);
	spListRemoveCurrent(source->innerList);

	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source) {
	return spListElementCopy(spBPQueueFirstElement(source));
}

SPListElement spBPQueuePeekLast(SPBPQueue source) {
	return spListElementCopy(spBPQueueLastElement(source));
}

double spBPQueueMinValue(SPBPQueue source) {
	return spListElementGetValue(spBPQueueFirstElement(source));
}

double spBPQueueMaxValue(SPBPQueue source) {
	return spListElementGetValue(spBPQueueLastElement(source));
}

bool spBPQueueIsEmpty(SPBPQueue source) {
	assert(source != NULL);
	return spBPQueueSize(source) == 0;
}

bool spBPQueueIsFull(SPBPQueue source) {
	assert(source != NULL);
	return spBPQueueSize(source) == spBPQueueGetMaxSize(source);
}

SPListElement spBPQueueFirstElement(SPBPQueue source) {
	if (source == NULL || spBPQueueIsEmpty(source)) {
		return NULL;
	}
	return spListGetFirst(source->innerList);
}

SPListElement spBPQueueLastElement(SPBPQueue source) {
	if (source == NULL || spBPQueueIsEmpty(source)) {
		return NULL;
	}
	return spListGetLast(source->innerList);
}



