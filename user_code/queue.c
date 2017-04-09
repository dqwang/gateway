#include "main.h"

Queue sq;//server queue
Queue lq;//lock queue

int sq_buf[SERVER_PACKET_NUM_MAX];
int lq_buf[LOCK_PACKET_NUM_MAX];



/* crateQueue function takes argument the maximum number of elements the Queue can hold, creates
a Queue according to it and returns a pointer to the Queue. */

#if 0
Queue * createQueue(int maxElements)
{
    /* Create a Queue */
    Queue *Q;
    Q = (Queue *)malloc(sizeof(Queue));
    /* Initialise its properties */
    Q->elements = (int *)malloc(sizeof(int)*maxElements);
    Q->size = 0;
    Q->capacity = maxElements;
    Q->front = 0;
    Q->rear = -1;
    /* Return the pointer */
    return Q;
}
#else
void createQueue(int maxElements, Queue *pQ, int *element_buf)
{
    /* Create a Queue */
    Queue *Q = pQ;
    
    /* Initialise its properties */
    Q->elements = element_buf;
    Q->size = 0;
    Q->capacity = maxElements;
    Q->front = 0;
    Q->rear = -1;
}

#endif


int is_queue_empty(Queue *Q)
{
	if(Q->size==0)
		return 1;
	else
		return 0;
}

int is_queue_full(Queue * Q)
{
	if(Q->size == Q->capacity)
		return 1;
	else
		return 0;
}

void Dequeue(Queue *Q)
{
	/* If Queue size is zero then it is empty. So we cannot pop */
	if(!is_queue_empty(Q)){	
		/* Removing an element is equivalent to incrementing index of front by one */
		Q->size--;
		Q->front++;
		/* As we fill elements in circular fashion */
		if(Q->front==Q->capacity){
			Q->front=0;
		}
	}	
}

int front(Queue *Q)
{
    if(!is_queue_empty(Q)){    
	    /* Return the element which is at the front*/
	    return Q->elements[Q->front];
    }
	
	return -1;
}
void Enqueue(Queue *Q,int element)
{
    /* If the Queue is full, we cannot push an element into it as there is no space for it.*/

	if(!is_queue_full(Q)){	
        Q->size++;
        Q->rear = Q->rear + 1;
        /* As we fill the queue in circular fashion */
        if(Q->rear == Q->capacity)
        {
        	Q->rear = 0;
        }
        /* Insert the element in its rear side */ 
        Q->elements[Q->rear] = element;
    } 
}

void queue_init(void)
{
	createQueue(SERVER_PACKET_NUM_MAX, &sq, sq_buf);
	createQueue(LOCK_PACKET_NUM_MAX, &lq, lq_buf);
}

