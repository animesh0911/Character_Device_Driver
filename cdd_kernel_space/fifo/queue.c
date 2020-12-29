#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kthread.h>

#include "queue.h"

static struct task_struct *thread;

Queue * initQueue(void)
{
    Queue *Q;
    Q = (Queue *)kmalloc(sizeof(Queue),GFP_KERNEL);
    Q->size = 0;
    INIT_LIST_HEAD(&Q->list);
    return Q;
}

QueueElement dequeue(Queue *Q)
{
    Queue* tmp;
    if(Q->size==0){
	printk(KERN_INFO"Queue is Empty.\n");
	return NULL;
    }
    Q->size--;
    tmp = list_entry(Q->list.next, Queue, list);
    list_del(Q->list.next);
    QueueElement e = tmp->e;
    kfree(tmp);
    return e;
}

int dequeue_thread(void *Q){
    Queue *q = (Queue*)Q;
    while(true){
	QueueElement e = dequeue(q);
	if(e!=NULL){
	    printk(KERN_INFO"\nData : %d",e);
	}
	else{
	    printk(KERN_INFO"\nQueueEmpty");
	    break;
	}
    }
    return 0;
}

void enqueue(Queue *Q, QueueElement element)
{
    Queue* newQ;
    Q->size++;
    newQ = (Queue*) kmalloc(sizeof(Queue),GFP_KERNEL);
    newQ->e = element;
    list_add_tail(&(newQ->list), &(Q->list));
}


static int __init test(void)
{
    Queue *testQueue = initQueue();
    enqueue(testQueue,1);
    enqueue(testQueue,2);
    enqueue(testQueue,3);
    enqueue(testQueue,4);
/*    printk(KERN_INFO"\nData : %d",dequeue(testQueue));
    printk(KERN_INFO"\nData : %d",dequeue(testQueue));
    printk(KERN_INFO"\nData : %d",dequeue(testQueue));
    printk(KERN_INFO"\nData : %d",dequeue(testQueue));
    printk(KERN_INFO"\nData : %d",dequeue(testQueue));
*/

    thread = kthread_run(dequeue_thread,(void*)testQueue,"Dequeue Thread");
    if(thread){
	printk(KERN_INFO"Kthread Created Successfully");
    }
    else{
	printk(KERN_INFO"Kthread Not Created");
    }

    return 0;
}

static void __exit test_exit(void){
    kthread_stop(thread);
    printk(KERN_INFO"\nDeleted");
}

module_init(test);
module_exit(test_exit);
EXPORT_SYMBOL(enqueue);

