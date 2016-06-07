
#include "../SPBPriorityQueue.h"
#include "unit_test_util.h"
#include <stdbool.h>

#define ASSERT(condition) ASSERT_TRUE(condition)

static bool listElementsEqualsNotSame3(SPListElement firstElement, SPListElement secondElement, SPListElement third);
static bool listElementsEqualsNotSame(SPListElement firstElement, SPListElement secondElement);

static bool successfulEnqueue(SPBPQueue queue, int index, double value);
static bool successfulEnqueueElement(SPBPQueue queue, SPListElement newElement);
static bool fullEnqueue(SPBPQueue queue, int index, double value);
static bool successfulDequeue(SPBPQueue queue);
static bool emptyDequeue(SPBPQueue queue);

static bool emptyQueue(SPBPQueue queue);
static bool queueState(SPBPQueue queue, int expectedSize, int minElementIndex, double minValue,
		int maxElementIndex, double maxValue);
static bool peekAndDequeueEquals(SPBPQueue queue1, SPBPQueue queue2, int index, double value);
static bool peekEquals(SPBPQueue queue, SPListElement element);
static bool peekLastEquals(SPBPQueue queue, SPListElement element);

static bool testQueueCreate() {

	ASSERT_NULL(spBPQueueCreate(0));
	ASSERT_NULL(spBPQueueCreate(-2));

	SPBPQueue queue = spBPQueueCreate(1);
	ASSERT_NOT_NULL(queue);
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 1);
	spBPQueueDestroy(queue);
	return true;
}

static bool testSingletonQueue() {
	SPBPQueue queue = spBPQueueCreate(1);
	ASSERT_NOT_NULL(queue);
	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, 2, 10.0));
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));

	ASSERT(fullEnqueue(queue, 2, 11.0));
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));
	ASSERT(fullEnqueue(queue, 3, 10.0));
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));
	ASSERT(fullEnqueue(queue, 2, 10.0));
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));

	ASSERT(successfulEnqueue(queue, 1, 10.0));
	ASSERT(queueState(queue, 1, 1, 10.0, 1, 10.0));

	ASSERT(successfulEnqueue(queue, 1, 9.999));
	ASSERT(queueState(queue, 1, 1, 9.999, 1, 9.999));

	ASSERT(fullEnqueue(queue, 1, 9.999));
	ASSERT(queueState(queue, 1, 1, 9.999, 1, 9.999));

	ASSERT(successfulDequeue(queue));
	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, 1, 10.0));
	ASSERT(queueState(queue, 1, 1, 10.0, 1, 10.0));

	spBPQueueClear(queue);
	ASSERT(emptyQueue(queue));

	spBPQueueDestroy(queue);

	return true;
}

static bool testMultipleQueueOperations() {
	SPBPQueue queue = spBPQueueCreate(3);
	ASSERT_NOT_NULL(queue);
	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, 2, 10.0)); // e1
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));

	ASSERT(successfulEnqueue(queue, 1, 10.0)); // e2
	ASSERT(queueState(queue, 2, 1, 10.0, 2, 10.0));

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 1, 2, 10.0, 2, 10.0));

	ASSERT(successfulDequeue(queue));
	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, 2, 1.0)); // e1
	ASSERT(queueState(queue, 1, 2, 1.0, 2, 1.0)); // Expected e1
	ASSERT(successfulEnqueue(queue, 1, 2.0)); // e2
	ASSERT(queueState(queue, 2, 2, 1.0, 1, 2.0)); // Expected e1->e2
	ASSERT(successfulEnqueue(queue, 3, 0.5)); // e3
	ASSERT(queueState(queue, 3, 3, 0.5, 1, 2.0)); // Expected e3->e1->e2

	ASSERT(successfulEnqueue(queue, 4, 1.23)); // e4
	ASSERT(queueState(queue, 3, 3, 0.5, 4, 1.23)); // Expected e3->e1->e4

	ASSERT(fullEnqueue(queue, 5, 1.23));
	ASSERT(queueState(queue, 3, 3, 0.5, 4, 1.23)); // Expected e3->e1->e4

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 2, 2, 1.0, 4, 1.23)); // Expected e1->e4

	ASSERT(successfulEnqueue(queue, 5, 1.23)); // e2
	ASSERT(queueState(queue, 3, 2, 1.0, 5, 1.23)); // Expected e1->e4->e2

	ASSERT(fullEnqueue(queue, 3, 3.0));
	ASSERT(queueState(queue, 3, 2, 1.0, 5, 1.23)); // Expected e1->e4->e2

	ASSERT(successfulEnqueue(queue, 1, 1.2)); // e3
	ASSERT(queueState(queue, 3, 2, 1.0, 4, 1.23)); // Expected e1->e3->e4

	ASSERT(fullEnqueue(queue, 10, 1.23));
	ASSERT(queueState(queue, 3, 2, 1.0, 4, 1.23)); // Expected e1->e3->e4

	ASSERT(successfulEnqueue(queue, 1, 1.1)); // e2
	ASSERT(queueState(queue, 3, 2, 1.0, 1, 1.2)); // Expected e1->e2->e3

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 2, 1, 1.1, 1, 1.2)); // Expected e2->e3

	ASSERT(successfulDequeue(queue));
	ASSERT(queueState(queue, 1, 1, 1.2, 1, 1.2)); // Expected e3

	spBPQueueClear(queue);

	ASSERT(emptyQueue(queue));

	spBPQueueDestroy(queue);

	return true;
}

static bool testDequeue() {

	ASSERT_SAME(spBPQueueDequeue(NULL), SP_BPQUEUE_INVALID_ARGUMENT);

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT(successfulEnqueue(queue, 2, 10.0)); // e1
	ASSERT(successfulEnqueue(queue, 5, 2.0)); // e2

	ASSERT(successfulDequeue(queue));
	ASSERT(successfulDequeue(queue));

	ASSERT(emptyDequeue(queue));
	ASSERT(emptyDequeue(queue));

	spBPQueueDestroy(queue);

	return true;
}

static bool testCopy() {
	ASSERT_NULL(spBPQueueCopy(NULL));
	SPBPQueue queue = spBPQueueCreate(3);
	SPBPQueue copy = spBPQueueCopy(queue);

	ASSERT_SAME(spBPQueueGetMaxSize(queue), spBPQueueGetMaxSize(copy));
	ASSERT_SAME(spBPQueueSize(queue), spBPQueueSize(copy));

	ASSERT(successfulEnqueue(queue, 1, 2)); // e1
	ASSERT(successfulEnqueue(queue, 2, 3)); // e2
	ASSERT(successfulEnqueue(queue, 3, 4)); // e3

	SPBPQueue copy2 = spBPQueueCopy(queue);

	ASSERT_SAME(spBPQueueSize(queue), 3);
	ASSERT_SAME(spBPQueueSize(copy), 0);
	ASSERT_SAME(spBPQueueSize(copy2), 3);

	ASSERT(peekAndDequeueEquals(queue, copy2, 1, 2));
	ASSERT(peekAndDequeueEquals(queue, copy2, 2, 3));
	ASSERT(peekAndDequeueEquals(queue, copy2, 3, 4));

	ASSERT(emptyQueue(queue));
	ASSERT(emptyQueue(copy2));

	ASSERT_SAME(spBPQueueSize(queue), 0);
	ASSERT_SAME(spBPQueueSize(copy2), 0);

	ASSERT(successfulEnqueue(queue, 2, 3));
	ASSERT(successfulEnqueue(queue, 3, 4));

	SPBPQueue copy3 = spBPQueueCopy(queue);
	ASSERT_SAME(spBPQueueSize(copy3), 2);

	ASSERT(peekAndDequeueEquals(queue, copy3, 2, 3));
	ASSERT(peekAndDequeueEquals(queue, copy3, 3, 4));

	ASSERT(emptyQueue(queue));
	ASSERT(emptyQueue(copy3));

	spBPQueueDestroy(queue);
	spBPQueueDestroy(copy);
	spBPQueueDestroy(copy2);
	spBPQueueDestroy(copy3);

	return true;

}

static bool testEnqueue() {

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT_NOT_NULL(queue);

	ASSERT_SAME(spBPQueueEnqueue(NULL, NULL), SP_BPQUEUE_INVALID_ARGUMENT);

	SPListElement e = spListElementCreate(2, 10.0);
	ASSERT_SAME(spBPQueueEnqueue(NULL, e), SP_BPQUEUE_INVALID_ARGUMENT);
	spListElementDestroy(e);

	ASSERT_SAME(spBPQueueEnqueue(queue, NULL), SP_BPQUEUE_INVALID_ARGUMENT);


	ASSERT(successfulEnqueue(queue, 2, 10.0)); // e1
	ASSERT(successfulEnqueue(queue, 5, 1.0)); // e3
	ASSERT(successfulEnqueue(queue, 5, 2.0)); // e2
	ASSERT(successfulEnqueue(queue, 3, 2.1)); // e4

	// The state of the queue should be e3->e2->e4->e1

	ASSERT(successfulEnqueue(queue, 7, 2.05)); // e5

	// The state of the queue should be e3->e2->e5->e4

	// Check a few not inserting scenarios

	ASSERT(fullEnqueue(queue, 100, 20.0));
	ASSERT(fullEnqueue(queue, 0, 15.0));
	ASSERT(fullEnqueue(queue, 7, 2.2));

	// The state of the queue should be e3->e2->e5->e4

	ASSERT(successfulEnqueue(queue, 6, 0.5)); // e1

	// The state of the queue should be e1->e3->e2->e5

	ASSERT(fullEnqueue(queue, 3, 2.06));

	ASSERT(successfulEnqueue(queue, 6, 2.05)); // e4

	// The state of the queue should be e1->e3->e2->e4

	ASSERT(successfulEnqueue(queue, 5, 1.0));

	// Insert same instance
	SPListElement e1 = spListElementCreate(6, 0.5);
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e1));
	spListElementDestroy(e1);

	ASSERT(fullEnqueue(queue, 5, 1.0));
	ASSERT(successfulEnqueue(queue, 6, 0.5));
	ASSERT(fullEnqueue(queue, 6, 0.5));

	spBPQueueDestroy(queue);

	return true;
}

static bool testQueueDestroy() {
	spBPQueueDestroy(NULL);
	return true;
}

static bool testClear() {
	spBPQueueClear(NULL);

	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT(emptyQueue(queue));

	ASSERT(successfulEnqueue(queue, 1, 1.0)); // e1
	ASSERT(successfulEnqueue(queue, 2, 2.0)); // e2

	ASSERT_SAME(spBPQueueSize(queue), 2);
	spBPQueueClear(queue);
	ASSERT(emptyQueue(queue));
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 3);

	ASSERT(successfulEnqueue(queue, 2, 2.0));
	ASSERT(successfulEnqueue(queue, 1, 1.0));

	ASSERT_SAME(spBPQueueSize(queue), 2);
	spBPQueueClear(queue);
	ASSERT(emptyQueue(queue));
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 3);

	spBPQueueDestroy(queue);

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

	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT(peekEquals(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT(peekEquals(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT(peekEquals(queue, e3));
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT(peekEquals(queue, e3));
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT(peekEquals(queue, e3));
	ASSERT(fullEnqueue(queue, 1, 2.0));
	ASSERT(peekEquals(queue, e3));
	ASSERT(fullEnqueue(queue, 2, 4.0));
	ASSERT(peekEquals(queue, e3));

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

	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT(peekLastEquals(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT(peekLastEquals(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT(peekLastEquals(queue, e1));
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT(peekLastEquals(queue, e2));
	ASSERT(successfulDequeue(queue));
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT(peekLastEquals(queue, e3));

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

	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_SAME(spBPQueueMinValue(queue), 2.0);
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT_SAME(spBPQueueMinValue(queue), 2.0);
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT_SAME(spBPQueueMinValue(queue), 1.99);
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_SAME(spBPQueueMinValue(queue), 1.99);
	ASSERT(successfulEnqueueElement(queue, e3));
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

	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);
	ASSERT(successfulEnqueueElement(queue, e2));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.1);
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.1);
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);
	ASSERT(successfulEnqueueElement(queue, e3));
	ASSERT_SAME(spBPQueueMaxValue(queue), 2.0);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;
}

static bool testEmpty() {
	SPBPQueue queue = spBPQueueCreate(2);
	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	SPListElement e1 = spListElementCreate(2, 2.0);
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	ASSERT(successfulDequeue(queue));
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	ASSERT(successfulDequeue(queue));
	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_FALSE(spBPQueueIsEmpty(queue));

	spBPQueueClear(queue);
	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	spBPQueueDestroy(queue);
	spListElementDestroy(e1);

	return true;
}

static bool testFull() {
	SPBPQueue queue = spBPQueueCreate(2);
	ASSERT_FALSE(spBPQueueIsFull(queue));

	SPListElement e1 = spListElementCreate(2, 2.0);
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_FALSE(spBPQueueIsFull(queue));
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_TRUE(spBPQueueIsFull(queue));
	ASSERT(fullEnqueue(queue, 2, 2.0));
	ASSERT_TRUE(spBPQueueIsFull(queue));
	ASSERT(successfulDequeue(queue));
	ASSERT_FALSE(spBPQueueIsFull(queue));
	ASSERT(successfulEnqueueElement(queue, e1));
	ASSERT_TRUE(spBPQueueIsFull(queue));

	spBPQueueDestroy(queue);
	spListElementDestroy(e1);

	return true;
}

static bool testElementCopy() {
	SPBPQueue queue = spBPQueueCreate(2);
	SPListElement element = spListElementCreate(2, 3);

	ASSERT(successfulEnqueueElement(queue, element));

	spListElementSetValue(element, 1);

	ASSERT_SAME(spBPQueueMinValue(queue), 3);

	spListElementSetValue(element, 3);
	SPListElement peekElement = spBPQueuePeek(queue);
	ASSERT(listElementsEqualsNotSame(element, peekElement));

	spBPQueueDestroy(queue);

	spListElementDestroy(element);
	spListElementDestroy(peekElement);

	return true;
}

/*** Helper assertion methods ***/

static bool peekEquals(SPBPQueue queue, SPListElement element) {
	SPListElement peekElement = spBPQueuePeek(queue);
	ASSERT(listElementsEqualsNotSame(peekElement, element));
	spListElementDestroy(peekElement);
	return true;
}

static bool peekLastEquals(SPBPQueue queue, SPListElement element) {
	SPListElement peekElement = spBPQueuePeekLast(queue);
	ASSERT(listElementsEqualsNotSame(peekElement, element));
	spListElementDestroy(peekElement);
	return true;
}

static bool peekAndDequeueEquals(SPBPQueue queue1, SPBPQueue queue2, int index, double value) {

	SPListElement e1 = spBPQueuePeek(queue1);
	spBPQueueDequeue(queue1);
	SPListElement e2 = spBPQueuePeek(queue2);
	spBPQueueDequeue(queue2);
	SPListElement e3 = spListElementCreate(index, value);
	ASSERT(listElementsEqualsNotSame3(e1, e2, e3));

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);

	return true;
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

static bool queueState(SPBPQueue queue, int expectedSize, int minElementIndex, double minValue,
		int maxElementIndex, double maxValue) {

	SPListElement expectedMinElement = spListElementCreate(minElementIndex, minValue);
	SPListElement expectedMaxElement = spListElementCreate(maxElementIndex, maxValue);

	ASSERT_SAME(spBPQueueSize(queue), expectedSize);

	SPListElement peekElement = spBPQueuePeek(queue);
	SPListElement peekLastElement = spBPQueuePeekLast(queue);

	ASSERT(listElementsEqualsNotSame(peekElement, expectedMinElement));
	ASSERT(listElementsEqualsNotSame(peekLastElement, expectedMaxElement));
	ASSERT_SAME(spBPQueueMinValue(queue), spListElementGetValue(expectedMinElement));
	ASSERT_SAME(spBPQueueMaxValue(queue), spListElementGetValue(expectedMaxElement));

	if (expectedSize == spBPQueueGetMaxSize(queue)) {
		ASSERT_TRUE(spBPQueueIsFull(queue));
	}

	spListElementDestroy(expectedMinElement);
	spListElementDestroy(expectedMaxElement);
	spListElementDestroy(peekElement);
	spListElementDestroy(peekLastElement);
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

static bool successfulEnqueue(SPBPQueue queue, int index, double value) {
	SPListElement newElement = spListElementCreate(index, value);
	ASSERT(successfulEnqueueElement(queue, newElement));
	spListElementDestroy(newElement);
	return true;
}

static bool successfulEnqueueElement(SPBPQueue queue, SPListElement newElement) {
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

	spListElementDestroy(e);
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
	RUN_TEST(testEmpty);
	RUN_TEST(testFull);
	RUN_TEST(testMultipleQueueOperations);
	RUN_TEST(testSingletonQueue);
	RUN_TEST(testElementCopy);
	return 0;
}


