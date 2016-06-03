
#include "../SPBPriorityQueue.h"
#include "unit_test_util.h"
#include <stdbool.h>

static bool assertListElementEqualButNotSame(SPListElement firstElement, SPListElement secondElement);

static bool enqueueAndAssertSuccessful(SPBPQueue queue, SPListElement newElement);
static bool enqueueAndAssertFull(SPBPQueue queue, int index, double value);
static bool dequeueAndAssertSuccessful(SPBPQueue queue);
static bool dequeueAndAssertEmpty(SPBPQueue queue);

static bool assertEmptyQueue(SPBPQueue queue);
static bool assertQueueState(SPBPQueue queue, int expectedSize, SPListElement expectedMinElement, SPListElement expectedMaxElement);


static bool testQueueCreate() {
	SPBPQueue queue = spBPQueueCreate(1);
	ASSERT_NOT_NULL(queue);
	spBPQueueDestroy(queue);
	return true;
}

static bool testQueueCreateSetsMaxSize() {
	SPBPQueue queue = spBPQueueCreate(1);
	ASSERT_NOT_NULL(queue);
	ASSERT_SAME(spBPQueueGetMaxSize(queue), 1);
	spBPQueueDestroy(queue);
	return true;
}

static bool testMultipleQueueOperations() {
	SPBPQueue queue = spBPQueueCreate(3);

	ASSERT_NOT_NULL(queue);

	assertEmptyQueue(queue);

	SPListElement e1 = spListElementCreate(2, 10.0);
	enqueueAndAssertSuccessful(queue, e1);
	assertQueueState(queue, 1, e1, e1);

	SPListElement e2 = spListElementCreate(1, 10.0);
	enqueueAndAssertSuccessful(queue, e2);
	assertQueueState(queue, 2, e2, e1);

	dequeueAndAssertSuccessful(queue);
	assertQueueState(queue, 1, e1, e1);

	dequeueAndAssertSuccessful(queue);
	assertEmptyQueue(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);

	e1 = spListElementCreate(2, 1.0);
	e2 = spListElementCreate(1, 2.0);
	SPListElement e3 = spListElementCreate(3, 0.5);
	SPListElement e4 = spListElementCreate(4, 1.23);

	enqueueAndAssertSuccessful(queue, e1);
	assertQueueState(queue, 1, e1, e1); // Expected e1
	enqueueAndAssertSuccessful(queue, e2);
	assertQueueState(queue, 2, e1, e2); // Expected e1->e2
	enqueueAndAssertSuccessful(queue, e3);
	assertQueueState(queue, 3, e3, e2); // Expected e3->e1->e2

	enqueueAndAssertSuccessful(queue, e4);
	assertQueueState(queue, 3, e3, e4); // Expected e3->e1->e4
	spListElementDestroy(e2);

	enqueueAndAssertFull(queue, 5, 1.23);
	assertQueueState(queue, 3, e3, e4); // Expected e3->e1->e4

	dequeueAndAssertSuccessful(queue);
	assertQueueState(queue, 2, e1, e4); // Expected e1->e4
	spListElementDestroy(e3);

	e2 = spListElementCreate(5, 1.23);
	enqueueAndAssertSuccessful(queue, e2);
	assertQueueState(queue, 3, e1, e2); // Expected e1->e4->e2

	enqueueAndAssertFull(queue, 3, 3.0);
	assertQueueState(queue, 3, e1, e2); // Expected e1->e4->e2

	e3 = spListElementCreate(1, 1.2);
	enqueueAndAssertSuccessful(queue, e3);
	assertQueueState(queue, 3, e1, e4); // Expected e1->e3->e4
	spListElementDestroy(e2);

	enqueueAndAssertFull(queue, 10, 1.23);
	assertQueueState(queue, 3, e1, e4); // Expected e1->e3->e4

	e2 = spListElementCreate(1, 1.1);
	enqueueAndAssertSuccessful(queue, e2); // Expected e1->e2->e3
	assertQueueState(queue, 3, e1, e3);

	dequeueAndAssertSuccessful(queue);
	assertQueueState(queue, 2, e2, e3); // Expected e2->e3

	dequeueAndAssertSuccessful(queue);
	assertQueueState(queue, 1, e3, e3); // Expected e3

	dequeueAndAssertSuccessful(queue);

	assertEmptyQueue(queue);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);

	return true;
}

static bool testDequeue() {
	SPBPQueue queue = spBPQueueCreate(4);

	SPListElement e1 = spListElementCreate(2, 10.0);
	SPListElement e2 = spListElementCreate(5, 2.0);

	enqueueAndAssertSuccessful(queue, e1);
	enqueueAndAssertSuccessful(queue, e2);


	dequeueAndAssertSuccessful(queue);
	dequeueAndAssertSuccessful(queue);

	dequeueAndAssertEmpty(queue);
	dequeueAndAssertEmpty(queue);

	spBPQueueDestroy(queue);

	spListElementDestroy(e1);
	spListElementDestroy(e2);

	return true;
}

static bool testEnqueue() {

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT_NOT_NULL(queue);
	SPListElement e1 = spListElementCreate(2, 10.0);
	SPListElement e2 = spListElementCreate(5, 2.0);
	SPListElement e3 = spListElementCreate(5, 1.0);
	SPListElement e4 = spListElementCreate(3, 2.1);

	enqueueAndAssertSuccessful(queue, e1);
	enqueueAndAssertSuccessful(queue, e3);
	enqueueAndAssertSuccessful(queue, e2);
	enqueueAndAssertSuccessful(queue, e4);

	// The state of the queue should be e3->e2->e4->e1

	SPListElement e5 = spListElementCreate(7, 2.05);
	enqueueAndAssertSuccessful(queue, e5);

	// The state of the queue should be e3->e2->e5->e4
	spListElementDestroy(e1);

	// Check a few not inserting scenarios

	enqueueAndAssertFull(queue, 100, 20.0);
	enqueueAndAssertFull(queue, 0, 15.0);
	enqueueAndAssertFull(queue, 7, 2.2);

	// The state of the queue should be e3->e2->e5->e4

	e1 = spListElementCreate(6, 0.5);
	enqueueAndAssertSuccessful(queue, e1);

	// The state of the queue should be e1->e3->e2->e5

	spListElementDestroy(e4);

	enqueueAndAssertFull(queue, 3, 2.06);

	e4 = spListElementCreate(6, 2.05);
	enqueueAndAssertSuccessful(queue, e4);

	// The state of the queue should be e1->e3->e2->e4

	// Destroy everything
	spBPQueueDestroy(queue);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	spListElementDestroy(e5);

	return true;

}

static bool dequeueAndAssertEmpty(SPBPQueue queue) {
	ASSERT_TRUE(spBPQueueIsEmpty(queue));
	ASSERT_SAME(spBPQueueDequeue(queue), SP_BPQUEUE_EMPTY);
	ASSERT_TRUE(spBPQueueIsEmpty(queue));
	return true;
}

static bool dequeueAndAssertSuccessful(SPBPQueue queue) {
	ASSERT_FALSE(spBPQueueIsEmpty(queue));
	int size = spBPQueueSize(queue);
	ASSERT_SAME(spBPQueueDequeue(queue), SP_BPQUEUE_SUCCESS);
	ASSERT_SAME(spBPQueueSize(queue), size - 1);
	return true;
}


static bool assertEmptyQueue(SPBPQueue queue) {
	ASSERT_TRUE(spBPQueueIsEmpty(queue));

	dequeueAndAssertEmpty(queue);

	// Assert basic properties
	ASSERT_NULL(spBPQueuePeek(queue));
	ASSERT_NULL(spBPQueuePeekLast(queue));
	ASSERT_SAME(spBPQueueMinValue(queue), -1);
	ASSERT_SAME(spBPQueueMaxValue(queue), -1);

	return true;
}

static bool assertQueueState(SPBPQueue queue, int expectedSize, SPListElement expectedMinElement, SPListElement expectedMaxElement) {
	ASSERT_SAME(spBPQueueSize(queue), expectedSize);
	assertListElementEqualButNotSame(spBPQueuePeek(queue), expectedMinElement);
	assertListElementEqualButNotSame(spBPQueuePeekLast(queue), expectedMaxElement);
	ASSERT_SAME(spBPQueueMinValue(queue), spListElementGetValue(expectedMinElement));
	ASSERT_SAME(spBPQueueMaxValue(queue), spListElementGetValue(expectedMaxElement));

	if (expectedSize == spBPQueueGetMaxSize(queue)) {
		ASSERT_TRUE(spBPQueueIsFull(queue));
	}
	return true;
}

static bool assertListElementEqualButNotSame(SPListElement firstElement, SPListElement secondElement) {
	ASSERT_NOT_SAME(firstElement, secondElement);
	ASSERT_SAME(spListElementCompare(firstElement, secondElement), 0);
	return true;
}

static bool enqueueAndAssertSuccessful(SPBPQueue queue, SPListElement newElement) {
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

static bool enqueueAndAssertFull(SPBPQueue queue, int index, double value) {
	ASSERT_TRUE(spBPQueueIsFull(queue));
	SPListElement e = spListElementCreate(index, value);
	ASSERT_SAME(spBPQueueEnqueue(queue, e), SP_BPQUEUE_FULL);
	ASSERT_TRUE(spBPQueueIsFull(queue));
	return true;
}

int main() {
	printf("Running SPBOQueueTest.. \n");
	RUN_TEST(testQueueCreate);
	RUN_TEST(testQueueCreateSetsMaxSize);
	RUN_TEST(testEnqueue);
	RUN_TEST(testDequeue);
	RUN_TEST(testMultipleQueueOperations);
	return 0;
}


