/* SPDX-License-Identifier: GPL-2.0 */
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/ns_common.h>
#include <linux/fs_pin.h>

struct mnt_namespace {
	struct ns_common	ns;
	struct mount *	root;
	/*
	 * Traversal and modification of .list is protected by either
	 * - taking namespace_sem for write, OR
	 * - taking namespace_sem for read AND taking .ns_lock.
	 */
	struct list_head	list;
	spinlock_t		ns_lock;
	struct user_namespace	*user_ns;
	struct ucounts		*ucounts;
	u64			seq;	/* Sequence number to prevent loops */
	wait_queue_head_t poll;
	u64 event;
	unsigned int		mounts; /* # of mounts in the namespace */
	unsigned int		pending_mounts;
} __randomize_layout;

struct mnt_pcp {
	int mnt_count;
	int mnt_writers;
};

struct mountpoint {
	struct hlist_node m_hash;
	struct dentry *m_dentry;
	struct hlist_head m_list;
	int m_count;
};

struct mount {
	struct hlist_node mnt_hash;  /* 用于链接到全局已挂载文件系统的链表 */
	struct mount *mnt_parent; /* 指向父对象，即文件系统1的mount实例 */
	struct dentry *mnt_mountpoint; /* 指向作为挂载点的目录，即文件系统1的目录a */
	struct vfsmount mnt;
	union {
		struct rcu_head mnt_rcu;
		struct llist_node mnt_llist;
	};
#ifdef CONFIG_SMP
	struct mnt_pcp __percpu *mnt_pcp;
#else
	int mnt_count;
	int mnt_writers;
#endif
	struct list_head mnt_mounts;	/* 挂载在此文件系统下的所有子文件系统的链表的表头，下面的节点都是mnt_child，list of children, anchored here */
	struct list_head mnt_child;	/* 链接到被此文件系统所挂的父文件系统的mnt_mounts上，and going through their mnt_child */
	struct list_head mnt_instance;	/* 链接到sb->s_mounts上的一个mount实例，mount instance on sb->s_mounts */
	const char *mnt_devname;	/* 设备名，如/dev/sdb1，Name of device e.g. /dev/dsk/hda1 */
	struct list_head mnt_list; /* 链接到进程namespace中已挂载文件系统中，表头为mnt_namespace的list域 */
	struct list_head mnt_expire;	/* 链接到一些文件系统专有的过期链表，如NFS, CIFS等， link in fs-specific expiry list */
	struct list_head mnt_share;	/* 链接到共享挂载的循环链表中，circular list of shared mounts */
	struct list_head mnt_slave_list;/* 此文件系统的slave mount链表的表头， list of slave mounts */
	struct list_head mnt_slave;	/* 连接到master文件系统的mnt_slave_list，slave list entry */
	struct mount *mnt_master;	/* 指向此文件系统的master文件系统，slave is on master->mnt_slave_list */
	struct mnt_namespace *mnt_ns;	/*  指向包含这个文件系统的进程的name space， containing namespace */
	struct mountpoint *mnt_mp;	/* where is it mounted */
	union {
		struct hlist_node mnt_mp_list;	/* list mounts with the same mountpoint */
		struct hlist_node mnt_umount;
	};
	struct list_head mnt_umounting; /* list entry for umount propagation */
#ifdef CONFIG_FSNOTIFY
	struct fsnotify_mark_connector __rcu *mnt_fsnotify_marks;
	__u32 mnt_fsnotify_mask;
#endif
	int mnt_id;			/* mount identifier */
	int mnt_group_id;		/* peer group identifier */
	int mnt_expiry_mark;		/* true if marked for expiry */
	struct hlist_head mnt_pins;
	struct hlist_head mnt_stuck_children;
} __randomize_layout;

#define MNT_NS_INTERNAL ERR_PTR(-EINVAL) /* distinct from any mnt_namespace */

static inline struct mount *real_mount(struct vfsmount *mnt)
{
	return container_of(mnt, struct mount, mnt);
}

static inline int mnt_has_parent(struct mount *mnt)
{
	return mnt != mnt->mnt_parent;
}

static inline int is_mounted(struct vfsmount *mnt)
{
	/* neither detached nor internal? */
	return !IS_ERR_OR_NULL(real_mount(mnt)->mnt_ns);
}

extern struct mount *__lookup_mnt(struct vfsmount *, struct dentry *);

extern int __legitimize_mnt(struct vfsmount *, unsigned);
extern bool legitimize_mnt(struct vfsmount *, unsigned);

static inline bool __path_is_mountpoint(const struct path *path)
{
	struct mount *m = __lookup_mnt(path->mnt, path->dentry);
	return m && likely(!(m->mnt.mnt_flags & MNT_SYNC_UMOUNT));
}

extern void __detach_mounts(struct dentry *dentry);

static inline void detach_mounts(struct dentry *dentry)
{
	if (!d_mountpoint(dentry))
		return;
	__detach_mounts(dentry);
}

static inline void get_mnt_ns(struct mnt_namespace *ns)
{
	refcount_inc(&ns->ns.count);
}

extern seqlock_t mount_lock;

struct proc_mounts {
	struct mnt_namespace *ns;
	struct path root;
	int (*show)(struct seq_file *, struct vfsmount *);
	struct mount cursor;
};

extern const struct seq_operations mounts_op;

extern bool __is_local_mountpoint(struct dentry *dentry);
static inline bool is_local_mountpoint(struct dentry *dentry)
{
	if (!d_mountpoint(dentry))
		return false;

	return __is_local_mountpoint(dentry);
}

static inline bool is_anon_ns(struct mnt_namespace *ns)
{
	return ns->seq == 0;
}

extern void mnt_cursor_del(struct mnt_namespace *ns, struct mount *cursor);
