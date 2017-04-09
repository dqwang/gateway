#ifndef __QUEUE_H__
#define __QUEUE_H__

/*Queue has five properties. capacity stands for the maximum number of elements Queue can hold.
  Size stands for the current size of the Queue and elements is the array of elements. front is the
 index of first element (the index at which we remove the element) and rear is the index of last element
 (the index at which we insert the element) */
typedef struct Queue
{
	int capacity;
	int size;
	int front;
	int rear;
	int *elements;
}Queue;

//Queue * createQueue(int maxElements);
void createQueue(int maxElements, Queue *pQ, int *element_buf);


int is_queue_empty(Queue *Q);

int is_queue_full(Queue * Q);


void Dequeue(Queue *Q);


int front(Queue *Q);

void Enqueue(Queue *Q,int element);

void queue_init(void);

#endif
