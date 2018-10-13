# include "queue.h"

// Creates a new queue
queue *newQueue(uint32_t size)
{
    queue *q = (queue *) malloc(sizeof(queue));
    q->size = size;
    q->head = 0;
    q->tail = 0;
    q->Q = (item *) calloc(size, sizeof(item));
    return q;
}

// Deletes a queue
void delQueue(queue *q)
{
    free(q->Q);
    free(q);
    return;
}

// Informs if the queue is full
bool emptyQ(queue *q)
{
    return q->head == q->tail;
}

// Informs if the queue is empty
bool fullQ(queue *q)
{
    return (q->head + 1) % q->size == q->tail % q->size;
}

// Add an item to the queue
bool enqueue(queue *q, item i)
{
    if (!fullQ(q)) // If the queue is not full
    {
        q->Q[q->head] = i; // Set the head of the queue to the item
        // Go through the entire queue
        for (uint32_t i = q->head; i != q->tail; i = (i + q->size - 1) % q->size)
        {
            // If the current item is greater than the next,
            // stop going through the queue
            if (q->Q[i] >= q->Q[(i + q->size - 1) % q->size])
            {
                break;
            }
            // Otherwise, swap the two items
            item temp = q->Q[i];
            q->Q[i] = q->Q[(i + q->size - 1) % q->size];
            q->Q[(i + q->size - 1) % q->size] = temp;
        }
        q->head = (q->head + 1) % q->size; // Move the head up by one slot

        return true;
    }
    return false; // Otherwise, enqueue failed
}

// Remove an item from the rear of the queue
bool dequeue(queue *q, item *i)
{
    if (!emptyQ(q)) // If the queue was not empty
    {
        *i = q->Q[q->tail]; // Set item to the one at the rear
        q->tail = (q->tail + 1) % q->size; // Move the rear up by one spot
        return true;
    }
    return false; // Otherwise, dequeue failed
}
