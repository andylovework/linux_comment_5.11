/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2018 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

#ifndef __YAFFS_LINUX_H__
#define __YAFFS_LINUX_H__

#include "yportenv.h"

struct yaffs_linux_context {
    struct list_head context_list; /* List of these we have mounted 通过该字段加入到yaffs_context_list全局链表中 */
    struct yaffs_dev *dev; /* 指向YAFFS文件系统超级块的指针 */
    struct super_block *super; /* 指向VFS层超级块的指针 */
    struct task_struct *bg_thread; /* Background thread for this device 后台垃圾回收线程的指针*/
    int bg_running; /* 启动和停止垃圾回收线程的标志位，1：启动，0：停止 */
    struct mutex gross_lock; /* Gross locking mutex 互斥锁，保护整个超级块关键字段的互斥访问，粒度比较大 */
    u8 *spare_buffer; /* For mtdif2 use. Don't know the buffer size at compile time so we have to allocate it. OOB块缓冲区 */
    struct list_head search_contexts; /* 通过该字段把所有Directory Search Context组成链表 */
    struct task_struct *readdir_process; /* 解决使用NFS死锁问题加入 */
    unsigned mount_id; /* 每个NAND flash分区挂载YAFFS都分配不同ID号 */
    int dirty;
};

#define yaffs_dev_to_lc(dev) ((struct yaffs_linux_context *)((dev)->os_context))
#define yaffs_dev_to_mtd(dev) ((struct mtd_info *)((dev)->driver_context))

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
#define WRITE_SIZE_STR "writesize"
#define WRITE_SIZE(mtd) ((mtd)->writesize)
#else
#define WRITE_SIZE_STR "oobblock"
#define WRITE_SIZE(mtd) ((mtd)->oobblock)
#endif

#endif
