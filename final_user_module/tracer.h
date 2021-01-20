#ifndef __TRACER_DRBD__
#define __TRACER_DRBD__

#ifdef __KERNEL__
#include <linux/spinlock_types.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/module.h>

#else
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <tracer.h>
#include <stdlib.h>
#include <string.h>
#define u8	unsigned char
#define u16	unsigned short
#define u32	unsigned int
#define u64	uint64_t
#endif

#define SHARED_SECRET_MAX 64
enum drbd_uuid_index {
	UI_CURRENT,
	UI_BITMAP,
	UI_HISTORY_START,
	UI_HISTORY_END,
	UI_SIZE,      /* nl-packet: number of dirty bits */
	UI_FLAGS,     /* nl-packet: flags */
	UI_EXTENDED_SIZE   /* Everything. */
};

#include "../drbd-8.4/drbd/drbd_protocol.h"

#ifdef __KERNEL__
#define drbdtracer_spinlock(spin)				\
	do {							\
		unsigned long tmp;				\
		spin_lock_irqsave(&(spin.drbd_sp_spin), tmp);	\
		spin.drbd_sp_flag = tmp;			\
	} while(0)

#define drbdtracer_unspinlock(spin)						\
	do {								\
		unsigned long tmp = spin.drbd_sp_flag;			\
		spin_unlock_irqrestore(&(spin.drbd_sp_spin), tmp);	\
	} while(0)

struct drbd_tracer_spin {
	spinlock_t	drbd_sp_spin;
	unsigned long	drbd_sp_flag;
};

struct drbd_tracer {
	int			mem_flag;
	struct trace_data	k_data;
	struct list_head	queue_list;
};

extern struct drbd_tracer *trace_dequeue_data (void);
#endif /* __KERNEL__ */

#define	DRBD_IOCTL	(('d' << 24) | ('r' << 16) | ('b' << 8))

#define TRACE_DRBD_DATA	(DRBD_IOCTL | 1)

struct user_data {
	unsigned long 		u_size;
	struct	trace_data	*u_data;
};

#endif
