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

// Method declaration
SPBPQueue spBPQueueInnerCreate(SPList innerList, int maxSize);
SPListElement spBPQueueFirstElement(SPBPQueue source);
SPListElement spBPQueueLastElement(SPBPQueue source);

struct sp_bp_queue_t {
	SPList innerList;
	SPListElement innerListMaxValueElement;
	int maxSize;
};

SPBPQueue spBPQueueCreate(int maxSize) {
	assert(maxSize > 0);
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
	assert(source != NULL);
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
	assert(source != NULL);
	return spListGetSize(source->innerList);
}

int spBPQueueGetMaxSize(SPBPQueue source) {
	assert(source != NULL);
	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element) {
	assert(source != NULL && element != NULL);

	SPListElement elementCopy = spListElementCopy(element);
	if (elementCopy == NULL) {
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}

	bool wasFull = spBPQueueIsFull(source);

	SPList list = source->innerList;

	bool wasInserted = false;
	// Advance iterator to the proper element's place
	SP_LIST_FOREACH(SPListElement, currentElement, list) {

		if (spListElementCompare(elementCopy, currentElement) < 0 && !wasInserted) {

			// Modifying while iterating - bad practice but is more efficient here.
			SP_LIST_MSG returnMSG = spListInsertBeforeCurrent(list, elementCopy);

			switch (returnMSG) {
			case SP_LIST_INVALID_CURRENT:
			case SP_LIST_NULL_ARGUMENT:
				spListElementDestroy(elementCopy);
				return SP_BPQUEUE_ERROR;
			case SP_LIST_OUT_OF_MEMORY:
				spListElementDestroy(elementCopy);
				return SP_BPQUEUE_OUT_OF_MEMORY;
			default:
				break;
			}
			wasInserted = true;
		}
	}

	if (wasInserted) {
		// Remove the last element if needed
		if (wasFull) {
			spListGetLast(list);
			SP_LIST_MSG returnMSG = spListRemoveCurrent(list);
			switch (returnMSG) {
			case SP_LIST_INVALID_CURRENT:
			case SP_LIST_NULL_ARGUMENT:
				spListElementDestroy(elementCopy);
				return SP_BPQUEUE_ERROR;
			default:
				break;
			}
		}
		return SP_BPQUEUE_SUCCESS;
	} else {
		if (spBPQueueIsFull(source)) {
			return SP_BPQUEUE_FULL;

		} else {
			// The item is largest than everything, insert last
			SP_LIST_MSG returnMSG = spListInsertLast(list, elementCopy);
			switch (returnMSG) {
			case SP_LIST_OUT_OF_MEMORY:
				spListElementDestroy(elementCopy);
				return SP_BPQUEUE_OUT_OF_MEMORY;
			default:
				return SP_BPQUEUE_SUCCESS;
			}
		}
	}
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	assert(source != NULL);

	if (spBPQueueIsEmpty(source)) {
		return SP_BPQUEUE_EMPTY;
	}

	spListGetFirst(source->innerList);
	SP_LIST_MSG returnMSG = spListRemoveCurrent(source->innerList);

	switch (returnMSG) {
	case SP_LIST_INVALID_CURRENT:
	case SP_LIST_NULL_ARGUMENT:
		return SP_BPQUEUE_ERROR;
	default:
		return SP_BPQUEUE_SUCCESS;
	}
}

SPListElement spBPQueuePeek(SPBPQueue source) {
	assert(source != NULL);

	SPListElement minElement = spBPQueueFirstElement(source);
	return minElement == NULL ? NULL : spListElementCopy(minElement);
}

SPListElement spBPQueuePeekLast(SPBPQueue source) {
	assert(source != NULL);

	SPListElement maxElement = spBPQueueLastElement(source);
	return maxElement == NULL ? NULL : spListElementCopy(maxElement);
}

double spBPQueueMinValue(SPBPQueue source) {
	assert(source != NULL);
	SPListElement minElement = spBPQueueFirstElement(source);
	return minElement == NULL ? -1 : spListElementGetValue(minElement);
}

double spBPQueueMaxValue(SPBPQueue source) {
	assert(source != NULL);
	SPListElement maxElement = spBPQueueLastElement(source);
	return maxElement == NULL ? -1 : spListElementGetValue(maxElement);
}

bool spBPQueueIsEmpty(SPBPQueue source) {
	assert(source != NULL);
	return spListGetSize(source->innerList) == 0;
}

bool spBPQueueIsFull(SPBPQueue source) {
	assert(source != NULL);
	return spListGetSize(source->innerList) == source->maxSize;
}

SPListElement spBPQueueFirstElement(SPBPQueue source) {
	assert(source != NULL);

	if (spBPQueueIsEmpty(source)) {
		return NULL;
	}
	return spListGetFirst(source->innerList);
}

SPListElement spBPQueueLastElement(SPBPQueue source) {

	if (spBPQueueIsEmpty(source)) {
		return NULL;
	}
	return spListGetLast(source->innerList);
}



