#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/kdev_t.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/ioctl.h>
#include<linux/list.h>

#define mem_size 1024

// Define the ioctl code
#define WR_DATA _IOW('a', 'a', int32_t*)
#define RD_DATA _IOR('a', 'b', int32_t*)

int32_t val = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;


static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static long chr_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = 
{
	.owner		= THIS_MODULE,
	.read		= my_read,
	.write		= my_write,
	.open		= my_open,
	.unlocked_ioctl	= chr_ioctl,
	.release	= my_release,
};

struct queue_node 
{
	struct list_head queue_list;
	int data;
};

struct list_head *head;

int enqueue(int data)
{
	struct queue_node *node;
	node = kmalloc(sizeof(struct queue_node*), GFP_KERNEL);
	if(node != NULL)
	{
		printk(KERN_INFO "Mem allocated\n");
		node->data = data;
		list_add_tail(&node->queue_list, head);
		return 0;
	}
	else
	{
		return -1;
	}
}

int dequeue()
{
	int res = -1;
	struct queue_node *node;	
		node = list_first_entry(head, struct queue_node, queue_list);
		res = node->data;
		list_del(&node->queue_list);
		return res;
}

static int my_open(struct inode *inode, struct file *file)
{
	/* creating physical memory */
	if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO"cannot allocate memory to the kernel...\n");
		return -1;
	}
	printk(KERN_INFO"Device file opened...\n");
	return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	printk(KERN_INFO"Device file closed...\n");
	return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	if(copy_to_user(buf, kernel_buffer, mem_size) == 0)
		printk(KERN_INFO"Data is read successfully...\n");
	return mem_size;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	if(copy_from_user(kernel_buffer, buf, len) == 0)
		printk(KERN_INFO"Data is written successfully...\n");
	return len;
}

static long chr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd)
	{
		case WR_DATA:
			if(copy_from_user(&val, (int32_t*)arg, sizeof(val)) != 0)
			{
				printk(KERN_INFO"ioctl write failed...\n");
				return 0;
			}
			printk(KERN_INFO"(ioctl) Value written = %d\n", val);
			break;
		case RD_DATA:
			if(copy_to_user((int32_t*)arg, &val, sizeof(val)) != 0)
			{
				printk(KERN_INFO"ioctl read failed...\n");
				return 0;
			}
			break;		
	}
	return 0;
}

static int __init chr_driver_init(void)
{
	

	/* Allocating Major Number */
	if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) < 0)
	{
		printk(KERN_INFO"Cannot allocate the major number...\n");
		return -1;
	}
	printk(KERN_INFO"Major = %d\nMinor = %d\n", MAJOR(dev), MINOR(dev));

	

	/* creating cdev structure */
	cdev_init(&my_cdev, &fops);
	
	
	
	/* adding character device to system */
	if((cdev_add(&my_cdev, dev, 1)) < 0)
	{
		printk(KERN_INFO"Cannot add the device to the system...\n");
		goto r_class;
	}
	
	
	
	/* creating struct class */
	if((dev_class = class_create(THIS_MODULE, "my_class")) == NULL)
	{
		printk(KERN_INFO"cannot create the struct class...\n");
		goto r_class;
	}

	
	/* creating device */
	if((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL)
	{
		printk(KERN_INFO"cannot create the device...\n");
		goto r_device;
	}
	printk(KERN_INFO"device driver is inserted...\n");
	//head = kmalloc(sizeof(struct list_head *), GFP_KERNEL);
	//INIT_LIST_HEAD(head);
	//enqueue(1);
	//enqueue(2);
	//printk(KERN_INFO "\n data = %d \n", dequeue());
	//printk(KERN_INFO "\n data = %d \n", dequeue());
	return 0;


r_device:
		class_destroy(dev_class);


r_class:
		unregister_chrdev_region(dev, 1);
		return -1;
}

void __exit chr_driver_exit(void)
{
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO"device driver is removed successfully...\n");	
}

module_init(chr_driver_init);
module_exit(chr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Animesh");
MODULE_DESCRIPTION("The character device driver");
