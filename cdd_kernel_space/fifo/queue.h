#include <linux/list.h>

#ifndef QueueElement
#define QueueElement void*
#endif

typedef struct{
    int size;
    QueueElement e;
    struct list_head list;
} Queue;

Queue * initQueue(void);
QueueElement dequeue(Queue *Q);
void enqueue(Queue *Q, QueueElement element);

