/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2008 Intel Corporation
 * Author: Matthew Wilcox <willy@linux.intel.com>
 *
 * Please see kernel/locking/semaphore.c for documentation of these functions
 */
#ifndef __LINUX_SEMAPHORE_H
#define __LINUX_SEMAPHORE_H

#include <linux/list.h>
#include <linux/spinlock.h>

/* Please don't access any members of this structure directly */
struct semaphore {
	raw_spinlock_t		lock; /* 自旋锁，保护信号量其他成员 */
	unsigned int		count; /* 计数器，表示还可以允许多少进程进入临界区 */
	struct list_head	wait_list; /* 等待进入临界区的进程链表 */
};
/* 指定名称和计数值，允许n个进程同时进入临界区 */
#define __SEMAPHORE_INITIALIZER(name, n)				\
{									\
	.lock		= __RAW_SPIN_LOCK_UNLOCKED((name).lock),	\
	.count		= n,						\
	.wait_list	= LIST_HEAD_INIT((name).wait_list),		\
}
/* 初始化一个互斥信号量 */
#define DEFINE_SEMAPHORE(name)	\
	struct semaphore name = __SEMAPHORE_INITIALIZER(name, 1)
/* 初始化信号量 */
static inline void sema_init(struct semaphore *sem, int val)
{
	static struct lock_class_key __key;
	*sem = (struct semaphore) __SEMAPHORE_INITIALIZER(*sem, val);
	lockdep_init_map(&sem->lock.dep_map, "semaphore->lock", &__key, 0);
}

extern void down(struct semaphore *sem); /* 获取信号量，如果计数值是0，进入深度睡眠 */
extern int __must_check down_interruptible(struct semaphore *sem); /* 获取信号量，如果计数值是0，进入轻度睡眠 */
extern int __must_check down_killable(struct semaphore *sem); /* 获取信号量，如果计数值是0，进入中度睡眠 */
extern int __must_check down_trylock(struct semaphore *sem); /* 获取信号量，如果计数值是0，进程不等待 */
extern int __must_check down_timeout(struct semaphore *sem, long jiffies); /* 获取信号量，指定等待时间 */
extern void up(struct semaphore *sem); /* 释放信号量， */

#endif /* __LINUX_SEMAPHORE_H */
