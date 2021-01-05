#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

extern void enqueue(void *element);

static int __init trial_init(void)
{
	enqueue(1);
	enqueue(2);
	return 0;
}

static void __exit trial_exit(void){
	printk(KERN_INFO"\nTrial Removed");
}

module_init(trial_init);
module_exit(trial_exit);

