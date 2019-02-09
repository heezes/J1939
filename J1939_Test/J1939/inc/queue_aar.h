#ifndef __QUEUE_AAR_H__
#define __QUEUE_AAR_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

typedef enum
{
	CHAR,
	INT, 
	FLOAT,
	DOUBLE
}TYPE;

// A structure to represent a queue
typedef struct
{
    volatile int front, rear, size;
	volatile uint8_t newline, queue_type;
    volatile unsigned capacity;
    volatile void* buffer_array;
}Queue;
 

 /*Function Prototype*/
//uint8_t createQueue(Queue* queue, void* buf, int size);
void create_queue(Queue* queue, void* buf, int size, TYPE t);
void enqueue(Queue* queue, void* item);
void* dequeue(Queue* queue);
int isFull(Queue* queue);
int isEmpty(Queue* queue);
void flush(Queue* queue);
#endif
