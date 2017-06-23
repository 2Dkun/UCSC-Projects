# ifndef _QUEUE_H
# define _QUEUE_H

# include "huffman.h"
# include <stdint.h>
# include <stdbool.h>
# include <stdlib.h>

typedef treeNode *item; // treeNode defined in huffman.h

typedef struct queue
{
	uint32_t size; // Size of queue
	uint32_t head, tail; // Front and rear locations
	item *Q; // Array to hold items
} queue;

queue *newQueue(uint32_t size); // Creates a new queue
void delQueue(queue *q); // Deletes a queue

bool emptyQ(queue *q); // Informs if the queue is empty
bool full(queue *q); // Informs if the queue is full

bool enqueue(queue *q, item i); // Add an item
bool dequeue(queue *q, item *i); // Remove from the rear

# endif
