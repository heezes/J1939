/*
Author: Altamash Abdul Rahim
Vecmocon Technologies Pvt Ltd.
*/
#include "queue_aar.h"

/*
 * Initializes a Queue of provided array.
 * */
#if 1
void create_queue(Queue* queue, void* buf, int size, TYPE t)
{	
	switch (t)
	{
		case CHAR:
			queue->capacity = (size/sizeof(char));
			queue->buffer_array = (char*)buf;
			queue->queue_type = CHAR;
			break;
		case INT:
			queue->capacity = (size/ sizeof(int));
			queue->buffer_array = (int*)buf;
			queue->queue_type = INT;
			break;
		case FLOAT:
			queue->capacity = (size/ sizeof(float));
			queue->buffer_array = (float*)buf;
			queue->queue_type = FLOAT;
			break;
		case DOUBLE:
			queue->capacity = (size/ sizeof(double));
			queue->buffer_array = (double*)buf;
			queue->queue_type = DOUBLE;
			break;
	}
	queue->front = queue->size = 0;
	queue->rear = (queue->capacity) - 1;
}
#endif

/* Queue is full when size becomes equal to the capacity
*/
int isFull(Queue* queue)
{  return (queue->size == queue->capacity);  }
 
/*Queue is empty when size is 0*/
int isEmpty(Queue* queue)
{  return (queue->size == 0); }
 
/*Function to add an item to the queue. */
void enqueue(Queue* queue, void* item)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1) % queue->capacity;
	if(queue->queue_type==CHAR)
		memcpy((char*)queue->buffer_array + queue->rear, (char*)item, 1);
	else if(queue->queue_type==INT)
		memcpy((int*)queue->buffer_array + queue->rear, (int*)item, sizeof(int));
	else if(queue->queue_type == FLOAT)
		memcpy((float*)queue->buffer_array + queue->rear, (float*)item, sizeof(float));
	else if(queue->queue_type == DOUBLE)
		memcpy((double*)queue->buffer_array + queue->rear, (double*)item, sizeof(double));
	queue->size = queue->size + 1;
}

/*Function to remove an item from queue.*/
void* dequeue(Queue* queue)
{
    if (isEmpty(queue)){
        return 0;}
	void* data = 0;
	if(queue->queue_type == CHAR)
		data = ((char*)queue->buffer_array + queue->front);
	else if(queue->queue_type == INT)
		data = ((int*)queue->buffer_array + queue->front);
	else if(queue->queue_type == FLOAT)
		data = ((float*)queue->buffer_array + queue->front);
	else if(queue->queue_type == DOUBLE)
		data = ((double*)queue->buffer_array + queue->front);
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return data;
}
 
void flush(Queue* queue)
{
	queue->front = queue->size = 0;
	queue->rear = (queue->capacity) - 1;
}
