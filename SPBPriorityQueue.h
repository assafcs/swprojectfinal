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
 * Insert a copy of the given element to the given queue and returns
 * a message informing the result of the operation
 *
 * If the queue is at full capacity, the element with the highest priority will be discarded from queue.
 * In case the given element is of the highest priority, it will not be inserted, and a SP_BPQUEUE_FULL
 * message will be returned
 *
 * @param source The queue to insert the element to
 * @param element The element to insert
 * @return
 *   SP_BPQUEUE_INVALID_ARGUMENT - In case a NULL was sent as the queue or element
 *   SP_BPQUEUE_OUT_OF_MEMORY - In case of memory allocation failure
 *   SP_BPQUEUE_FULL - In case the queue is at full capacity, and the given element's priority
 *   				   is higher than all of the existing elements in the queue
 *   SP_BPQUEUE_SUCCESS - In case the element was inserted to the queue (even if another element
 *   					  was discarded as a result of maximum capacity)
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removed the element with the lowest priority from the queue.
 *
 * If the queue is empty, nothing will happen and a proper message will be returned
 *
 * @param source The queue to dequeue from
 * @return
 * 	 SP_BPQUEUE_INVALID_ARGUMENT - In case a NULL was sent as the queue
 * 	 SP_BPQUEUE_EMPTY - In case the queue is empty
 * 	 SP_BPQUEUE_SUCCESS - In case the dequeue was performed successfully.
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns a copy of the element with the lowest priority.
 *
 * @param source The queue to peek into
 * @return
 *  NULL in case the given queue is NULL, empty, or a memory allocation failure occurred
 * 	The element with the lowest priority otherwise.
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns a copy of the element with the highest priority.
 *
 * @param source The queue to peek into
 * @return
 *  NULL in case the given queue is NULL, empty, or a memory allocation failure occurred
 * 	The element with the highest priority otherwise.
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * Returns the minimum value of elements in the queue
 *
 * @param source The queue for which the minimum value is requested
 * @return
 *  -1 in case the given queue is NULL of empty
 *  The minimum value of elements in the queue otherwise.
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * Returns the maximum value of elements in the queue
 *
 * @param source The queue for which the maximum value is requested
 * @return
 *  -1 in case the given queue is NULL of empty
 *  The maximum value of elements in the queue otherwise.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Returns whether the queue is considered empty or not.
 *
 * @param source The queue to check for emptiness. NULL is not allowed!
 * @return
 *  True if the queue is considered empty, false otherwise
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Returns whether the queue is considered full or not, that is, if the queue is at full capacity.
 *
 * @param source The queue to check for fullness. NULL is not allowed!
 * @return
 *  True if the queue is considered full, false otherwise
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
