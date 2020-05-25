#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

struct queue
{
	/* TODO Phase 1 */

	// Want an array of items
	void **items;
	int front;
	int rear;
	int length;
};

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	// Allocate an empty queue
	struct queue *newQueue = (struct queue *)malloc(sizeof(struct queue));
	newQueue->length = 0;
	newQueue->front = -1;
	newQueue->rear = -1;

	newQueue->items = (void **)malloc(sizeof(void *) * 20);

	if (newQueue->items == NULL || newQueue == NULL)
	{
		return NULL;
	}

	// retrieve address
	queue_t addrNewQueue = newQueue;

	return addrNewQueue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL)
	{
		return -1;
	}
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */

	// return -1 if @queue or @data are NULL
	if (queue == NULL || data == NULL)
	{
		return -1;
	}

	if (queue->front == -1 && queue->rear == -1)
	{
		queue->front = 0;
		queue->rear = 0;
	}
	else
	{
		// increment rear first
		queue->rear = (queue->rear + 1);
	}

	queue->items[queue->rear] = data;
	queue->length = queue->length + 1;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */

	//Return: -1 if @queue or @data are NULL, or if the queue is empty
	//0 if @data was set with the oldest item available in @queue.
	if (queue == NULL || data == NULL || (queue->front == -1 && queue->rear == -1))
	{
		return -1;
	}

	// increment front
	*data = queue->items[queue->front];

	if (queue->front == queue->rear)
	{
		queue->front = -1;
		queue->rear = -1;
	}
	else
	{

		queue->front = (queue->front + 1);
	}
	queue->length = queue->length - 1;

	return 0;
}

// Doesn't have to be O(1)
int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */

	if (queue == NULL || data == NULL)
	{
		return -1;
	}
	for (int i = queue->front; i <= queue->rear; i++)
	{
		// find if data == item i
		void *currentItem = queue->items[i];
		if (data == currentItem)
		{
			// remove this
			for (int c = i; c <= queue->rear; c++)
				queue->items[c] = queue->items[c + 1];
			queue->rear = queue->rear - 1;
			queue->length = queue->length - 1;

			return 0;
		}
	}

	return -1;
}

// Doesn't have to be O(1)
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	/* TODO Phase 1 */
	if (queue == NULL || func == NULL)
	{
		return -1;
	}
	for (int i = queue->front; i <= queue->rear; i++)
	{

		void *currentItem = queue->items[i];
		//int currentVal = *((int *)currentItem);
		//printf("%d, Before function call: %d\n", i, currentVal);
		int functionCall = func(currentItem, arg);
		//int afterVal = *((int *)currentItem);
		//printf("%d, After function call: %d\n", i, afterVal);

		//If @func returns 1 for a specific item, the iteration stops prematurely.
		if (functionCall == 1)
		{
			if (currentItem != NULL)
			{
				*data = currentItem;
			}
			break;
		}
	}
	// calling func

	return 0;
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
	if (queue == NULL)
	{
		return -1;
	}
	return queue->length;
}