
#include "../SPBPriorityQueue.h"
#include "unit_test_util.h"
#include <stdbool.h>

#define ASSERT(condition) ASSERT_TRUE(condition)

static bool listElementsEqualsNotSame3(SPListElement firstElement, SPListElement secondElement, SPListElement third);
static bool listElementsEqualsNotSame(SPListElement firstElement, SPListElement secondElement);

static bool successfulEnqueue(SPBPQueue queue, SPListElement newElement);
static bool fullEnqueue(SPBPQueue queue, int index, double value);
static bool successfulDequeue(SPBPQueue queue);
static bool emptyDequeue(SPBPQueue queue);

static bool emptyQueue(SPBPQueue queue);
static bool queueState(SPBPQueue queue, int expectedSize, SPListElement expectedMinElement, SPListElement expectedMaxElement);

static SPListElement peekAndDequeue(SPBPQueue queue);

static bool testQueueCreate() {

	ASSERT_NULL(spBPQueueCreate(0));
	ASSERT_NULL(spBPQueueCreate(-2));

	SPBPQueue queue = spBPQueueCreate(1);
	ASSERT_NOT_NULL(queue);
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 1);
	spBPQueueDestroy(queue);
	return true;
}

static bool testMultipleQueueOperations() {
	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT_NOT_NULL(queue);

	ASSERT(emptyQueue(queue));

	SPListElement e1 = spListElementCreate(2, 10.0);
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(queueState(queue, 1, e1, e1));

	SPListElement e2 = spListElementCreate(1, 10.0);
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(queueState(queue, 2, e2, e1));

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 1, e1, e1));

	ASSERT(successfulDequeue(queue));
	ASSERT(emptyQueue(queue));

	spListElementDestroy(e1);
	spListElementDestroy(e2);

	e1 = spListElementCreate(2, 1.0);
	e2 = spListElementCreate(1, 2.0);
	SPListElement e3 = spListElementCreate(3, 0.5);
	SPListElement e4 = spListElementCreate(4, 1.23);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(queueState(queue, 1, e1, e1)); // Expected e1
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(queueState(queue, 2, e1, e2)); // Expected e1->e2
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(queueState(queue, 3, e3, e2)); // Expected e3->e1->e2

	ASSERT(successfulEnqueue(queue, e4));
	ASSERT(queueState(queue, 3, e3, e4)); // Expected e3->e1->e4
	spListElementDestroy(e2);

	ASSERT(fullEnqueue(queue, 5, 1.23));
	ASSERT(queueState(queue, 3, e3, e4)); // Expected e3->e1->e4

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 2, e1, e4)); // Expected e1->e4
	spListElementDestroy(e3);

	e2 = spListElementCreate(5, 1.23);
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(queueState(queue, 3, e1, e2)); // Expected e1->e4->e2

	ASSERT(fullEnqueue(queue, 3, 3.0));
	ASSERT(queueState(queue, 3, e1, e2)); // Expected e1->e4->e2

	e3 = spListElementCreate(1, 1.2);
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(queueState(queue, 3, e1, e4)); // Expected e1->e3->e4
	spListElementDestroy(e2);

	ASSERT(fullEnqueue(queue, 10, 1.23));
	ASSERT(queueState(queue, 3, e1, e4)); // Expected e1->e3->e4

	e2 = spListElementCreate(1, 1.1);
	ASSERT(successfulEnqueue(queue, e2)); // Expected e1->e2->e3
	ASSERT(queueState(queue, 3, e1, e3));

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 2, e2, e3)); // Expected e2->e3

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 1, e3, e3)); // Expected e3

	spBPQueueClear(queue);

	ASSERT(emptyQueue(queue));

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);

	return true;
}

static bool testDequeue() {

	ASSERT_SAME(spBPQueueDequeue(NULL), SP_BPQUEUE_INVALID_ARGUMENT);

	SPBPQueue queue = spBPQueueCreate(4);

	SPListElement e1 = spListElementCreate(2, 10.0);
	SPListElement e2 = spListElementCreate(5, 2.0);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(successfulEnqueue(queue, e2));


	ASSERT(successfulDequeue(queue));
	ASSERT(successfulDequeue(queue));

	ASSERT(emptyDequeue(queue));
	ASSERT(emptyDequeue(queue));

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);

	return true;
}

static bool testCopy() {
	ASSERT_NULL(spBPQueueCopy(NULL));
	SPBPQueue queue = spBPQueueCreate(3);
	SPBPQueue copy = spBPQueueCopy(queue);

	ASSERT_SAME(spBPQueueGetMaxSize(queue), spBPQueueGetMaxSize(copy));
	ASSERT_SAME(spBPQueueSize(queue), spBPQueueSize(copy));

	SPListElement e1 = spListElementCreate(1,2);
	SPListElement e2 = spListElementCreate(2,3);
	SPListElement e3 = spListElementCreate(3,4);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(successfulEnqueue(queue, e3));

	SPBPQueue copy2 = spBPQueueCopy(queue);

	ASSERT_SAME(spBPQueueSize(queue), 3);
	ASSERT_SAME(spBPQueueSize(copy), 0);
	ASSERT_SAME(spBPQueueSize(copy2), 3);

	ASSERT(listElementsEqualsNotSame3(peekAndDequeue(queue), peekAndDequeue(copy2), e1));
	ASSERT(listElementsEqualsNotSame3(peekAndDequeue(queue), peekAndDequeue(copy2), e2));
	ASSERT(listElementsEqualsNotSame3(peekAndDequeue(queue), peekAndDequeue(copy2), e3));

	ASSERT(emptyQueue(queue));
	ASSERT(emptyQueue(copy2));

	ASSERT_SAME(spBPQueueSize(queue), 0);
	ASSERT_SAME(spBPQueueSize(copy2), 0);

	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(successfulEnqueue(queue, e3));


	SPBPQueue copy3 = spBPQueueCopy(queue);
	ASSERT_SAME(spBPQueueSize(copy3), 2);

	ASSERT(listElementsEqualsNotSame3(peekAndDequeue(queue), peekAndDequeue(copy3), e2));
	ASSERT(listElementsEqualsNotSame3(peekAndDequeue(queue), peekAndDequeue(copy3), e3));

	ASSERT(emptyQueue(queue));
	ASSERT(emptyQueue(copy3));

	spBPQueueDestroy(queue);
	spBPQueueDestroy(copy);
	spBPQueueDestroy(copy2);
	spBPQueueDestroy(copy3);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;

}

static bool testEnqueue() {

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT_NOT_NULL(queue);
	SPListElement e1 = spListElementCreate(2, 10.0);
	SPListElement e2 = spListElementCreate(5, 2.0);
	SPListElement e3 = spListElementCreate(5, 1.0);
	SPListElement e4 = spListElementCreate(3, 2.1);

	ASSERT_SAME(spBPQueueEnqueue(NULL, NULL), SP_BPQUEUE_INVALID_ARGUMENT);
	ASSERT_SAME(spBPQueueEnqueue(NULL, e1), SP_BPQUEUE_INVALID_ARGUMENT);
	ASSERT_SAME(spBPQueueEnqueue(queue, NULL), SP_BPQUEUE_INVALID_ARGUMENT);


	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(successfulEnqueue(queue, e4));

	// The state of the queue should be e3->e2->e4->e1

	SPListElement e5 = spListElementCreate(7, 2.05);
	ASSERT(successfulEnqueue(queue, e5));

	// The state of the queue should be e3->e2->e5->e4
	spListElementDestroy(e1);

	// Check a few not inserting scenarios

	ASSERT(fullEnqueue(queue, 100, 20.0));
	ASSERT(fullEnqueue(queue, 0, 15.0));
	ASSERT(fullEnqueue(queue, 7, 2.2));

	// The state of the queue should be e3->e2->e5->e4

	e1 = spListElementCreate(6, 0.5);
	ASSERT(successfulEnqueue(queue, e1));

	// The state of the queue should be e1->e3->e2->e5

	spListElementDestroy(e4);

	ASSERT(fullEnqueue(queue, 3, 2.06));

	e4 = spListElementCreate(6, 2.05);
	ASSERT(successfulEnqueue(queue, e4));

	// The state of the queue should be e1->e3->e2->e4

	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(fullEnqueue(queue, 5, 1.0));
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(fullEnqueue(queue, 6, 0.5));

	// Destroy everything
	spBPQueueDestroy(queue);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	spListElementDestroy(e5);

	return true;

}

static bool testQueueDestroy() {
	spBPQueueDestroy(NULL);
	return true;
}

static bool testClear() {
	spBPQueueClear(NULL);

	SPBPQueue queue = spBPQueueCreate(3);

	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);

	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(successfulEnqueue(queue, e2));

	ASSERT_SAME(spBPQueueSize(queue), 2);
	spBPQueueClear(queue);
	ASSERT(emptyQueue(queue));
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 3);

	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(successfulEnqueue(queue, e1));

	ASSERT_SAME(spBPQueueSize(queue), 2);
	spBPQueueClear(queue);
	ASSERT(emptyQueue(queue));
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 3);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);

	return true;

}

static bool testPeek() {
	ASSERT_NULL(spBPQueuePeek(NULL));

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT_NULL(spBPQueuePeek(queue));

	SPListElement e1 = spListElementCreate(1, 2.0);
	SPListElement e2 = spListElementCreate(2, 3.0);
	SPListElement e3 = spListElementCreate(2, 1.0);
	SPListElement e4 = spListElementCreate(2, 4.0);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e1));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e1));
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e3));
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e3));
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e3));
	ASSERT(fullEnqueue(queue, 1, 2.0));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e3));
	ASSERT(fullEnqueue(queue, 2, 4.0));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), e3));

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testPeekLast() {

	ASSERT_NULL(spBPQueuePeekLast(NULL));

	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT_NULL(spBPQueuePeekLast(queue));

	SPListElement e1 = spListElementCreate(2, 2.0);
	SPListElement e2 = spListElementCreate(1, 2.0);
	SPListElement e3 = spListElementCreate(0, 2.01);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), e1));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), e1));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), e1));
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), e2));
	ASSERT(successfulDequeue(queue));
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), e3));

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;

}

static bool testMinValue() {

	ASSERT_SAME(spBPQueueMinValue(NULL), -1);

	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT_SAME(spBPQueueMinValue(queue), -1);

	SPListElement e1 = spListElementCreate(2, 2.0);
	SPListElement e2 = spListElementCreate(1, 2.1);
	SPListElement e3 = spListElementCreate(3, 1.99);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT_SAME(spBPQueueMinValue(queue), 2.0);
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT_SAME(spBPQueueMinValue(queue), 2.0);
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT_SAME(spBPQueueMinValue(queue), 1.99);
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT_SAME(spBPQueueMinValue(queue), 1.99);
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT_SAME(spBPQueueMinValue(queue), 1.99);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;
}

static bool testMaxValue() {

	ASSERT_SAME(spBPQueueMaxValue(NULL), -1);

	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT_SAME(spBPQueueMaxValue(queue), -1);

	SPListElement e1 = spListElementCreate(2, 2.0);
	SPListElement e2 = spListElementCreate(1, 2.1);
	SPListElement e3 = spListElementCreate(3, 1.99);

	ASSERT(successfulEnqueue(queue, e1));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);
	ASSERT(successfulEnqueue(queue, e2));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.1);
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.1);
	ASSERT(successfulEnqueue(queue, e1));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);
	ASSERT(successfulEnqueue(queue, e3));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;

}

/*** Helper assertion methods ***/

static SPListElement peekAndDequeue(SPBPQueue queue) {
	SPListElement e = spBPQueuePeek(queue);
	spBPQueueDequeue(queue);
	return e;
}

static bool emptyDequeue(SPBPQueue queue) {
	ASSERT_TRUE(spBPQueueIsEmpty(queue));
	ASSERT_SAME(spBPQueueDequeue(queue), SP_BPQUEUE_EMPTY);
	ASSERT_TRUE(spBPQueueIsEmpty(queue));
	return true;
}

static bool successfulDequeue(SPBPQueue queue) {
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	int size = spBPQueueSize(queue);
	ASSERT_SAME(spBPQueueDequeue(queue), SP_BPQUEUE_SUCCESS);
	ASSERT_SAME(spBPQueueSize(queue), size - 1);
	return true;
}


static bool emptyQueue(SPBPQueue queue) {
	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	ASSERT(emptyDequeue(queue));

	// Assert basic properties
	ASSERT_NULL(spBPQueuePeek(queue));
	ASSERT_NULL(spBPQueuePeekLast(queue));
	ASSERT_SAME(spBPQueueMinValue(queue), -1);
	ASSERT_SAME(spBPQueueMaxValue(queue), -1);

	return true;
}

static bool queueState(SPBPQueue queue, int expectedSize, SPListElement expectedMinElement, SPListElement expectedMaxElement) {
	ASSERT_SAME(spBPQueueSize(queue), expectedSize);
	ASSERT(listElementsEqualsNotSame(spBPQueuePeek(queue), expectedMinElement));
	ASSERT(listElementsEqualsNotSame(spBPQueuePeekLast(queue), expectedMaxElement));
	ASSERT_SAME(spBPQueueMinValue(queue), spListElementGetValue(expectedMinElement));
	ASSERT_SAME(spBPQueueMaxValue(queue), spListElementGetValue(expectedMaxElement));

	if (expectedSize == spBPQueueGetMaxSize(queue)) {
		ASSERT_TRUE(spBPQueueIsFull(queue));
	}
	return true;
}

static bool listElementsEqualsNotSame(SPListElement firstElement, SPListElement secondElement) {
	ASSERT_NOT_SAME(firstElement, secondElement);
	ASSERT_SAME(spListElementCompare(firstElement, secondElement), 0);
	return true;
}

static bool listElementsEqualsNotSame3(SPListElement firstElement, SPListElement secondElement, SPListElement thirdElement) {
	ASSERT(listElementsEqualsNotSame(firstElement, secondElement));
	ASSERT(listElementsEqualsNotSame(firstElement, thirdElement));
	ASSERT(listElementsEqualsNotSame(secondElement, thirdElement));
	return true;
}

static bool successfulEnqueue(SPBPQueue queue, SPListElement newElement) {
	if (spBPQueueIsFull(queue)) {
		ASSERT_SAME(spBPQueueEnqueue(queue, newElement), SP_BPQUEUE_SUCCESS);
		ASSERT_TRUE(spBPQueueIsFull(queue));
	} else {
		int size = spBPQueueSize(queue);
		ASSERT_SAME(spBPQueueEnqueue(queue, newElement), SP_BPQUEUE_SUCCESS);
		ASSERT_SAME(spBPQueueSize(queue), size + 1);
	}
	return true;
}

static bool fullEnqueue(SPBPQueue queue, int index, double value) {
	ASSERT_TRUE(spBPQueueIsFull(queue));
	SPListElement e = spListElementCreate(index, value);
	ASSERT_SAME(spBPQueueEnqueue(queue, e), SP_BPQUEUE_FULL);
	ASSERT_TRUE(spBPQueueIsFull(queue));
	return true;
}

int main() {
	printf("Running SPBOQueueTest.. \n");
	RUN_TEST(testQueueCreate);
	RUN_TEST(testCopy);
	RUN_TEST(testQueueDestroy);
	RUN_TEST(testClear);
	RUN_TEST(testEnqueue);
	RUN_TEST(testDequeue);
	RUN_TEST(testPeek);
	RUN_TEST(testPeekLast);
	RUN_TEST(testMinValue);
	RUN_TEST(testMaxValue);
	RUN_TEST(testMultipleQueueOperations);
	return 0;
}


