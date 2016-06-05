#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new bounded priority queue.
 *
 * @param maxSize the maximum capacity of the queue
 * @return
 * 	NULL - If allocations failed.
 * 	A new queue in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of the given queue.
 *
 * The new copy will contain all the elements from the source queue in the same
 * priority order.
 *
 * @param source The queue to copy
 * @return
 *  NULL if a NULL was sent or a memory allocation failed.
 *  Otherwise, a bounded priority queue containing the same elements with same priority order.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates the given queue. Clears all elements by using the stored free function.
 *
 * @param source The queue to be deallocated. If queue is NULL nothing will be done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from the given queue.
 *
 * The elements are deallocated using the stored freeing function
 *
 * @param source The queue to remove all element from. If queue is NULL nothing will be done
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in the queue.
 *
 * @param source The queue for which size is requested.
 * @return
 *  -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum capacity of the queue.
 *
 * @param source The queue for which maximum capacity is requested.
 * @return
 *  -1 if a NULL pointer was sent.
 * Otherwise the maximum capacity the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * TODO Complete documentation
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * TODO Complete documentation
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * TODO Complete documentation
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * TODO Complete documentation
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * TODO Complete documentation
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * TODO Complete documentation
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * TODO Complete documentation
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * TODO Complete documentation
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
