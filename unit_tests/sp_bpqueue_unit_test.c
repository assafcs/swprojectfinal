
#include "../SPBPriorityQueue.h"
#include "unit_test_util.h"
#include <stdbool.h>

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

static bool testEnqueue() {

	SPBPQueue queue = spBPQueueCreate(4);

	ASSERT_NOT_NULL(queue);
	SPListElement e1 = spListElementCreate(2, 10.0);
	SPListElement e2 = spListElementCreate(5, 2.0);
	SPListElement e3 = spListElementCreate(5, 1.0);
	SPListElement e4 = spListElementCreate(3, 2.1);

	ASSERT_SAME(spBPQueueEnqueue(queue, e1), SP_BPQUEUE_SUCCESS);
	ASSERT_SAME(spBPQueueEnqueue(queue, e3), SP_BPQUEUE_SUCCESS);
	ASSERT_SAME(spBPQueueEnqueue(queue, e2), SP_BPQUEUE_SUCCESS);
	ASSERT_SAME(spBPQueueEnqueue(queue, e4), SP_BPQUEUE_SUCCESS);

	// The state of the queue should be e3->e2->e4->e1

	SPListElement e5 = spListElementCreate(7, 2.05);
	ASSERT_SAME(spBPQueueEnqueue(queue, e5), SP_BPQUEUE_SUCCESS);

	// The state of the queue should be e3->e2->e5->e4
	spListElementDestroy(e1);

	// Check a few not inserting scenarios

	e1 = spListElementCreate(100, 20.0);
	ASSERT_SAME(spBPQueueEnqueue(queue, e1), SP_BPQUEUE_FULL);
	spListElementDestroy(e1);

	e1 =  spListElementCreate(0, 15.0);
	ASSERT_SAME(spBPQueueEnqueue(queue, e1), SP_BPQUEUE_FULL);
	spListElementDestroy(e1);

	e1 = spListElementCreate(7, 2.2);
	ASSERT_SAME(spBPQueueEnqueue(queue, e1), SP_BPQUEUE_FULL);
	spListElementDestroy(e1);

	// The state of the queue should be e3->e2->e5->e4

	e1 = spListElementCreate(6, 0.5);
	ASSERT_SAME(spBPQueueEnqueue(queue, e1), SP_BPQUEUE_SUCCESS);

	// The state of the queue should be e1->e3->e2->e5

	spListElementDestroy(e4);

	e4 = spListElementCreate(3, 2.06);
	ASSERT_SAME(spBPQueueEnqueue(queue, e4), SP_BPQUEUE_FULL);
	spListElementDestroy(e4);

	e4 = spListElementCreate(6, 2.05);
	ASSERT_SAME(spBPQueueEnqueue(queue, e4), SP_BPQUEUE_SUCCESS);

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

int main() {
	printf("Running SPBOQueueTest.. \n");
	RUN_TEST(testQueueCreate);
	RUN_TEST(testQueueCreateSetsMaxSize);
	RUN_TEST(testEnqueue);
	return 0;
}


