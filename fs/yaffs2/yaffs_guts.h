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

#ifndef __YAFFS_GUTS_H__
#define __YAFFS_GUTS_H__

#include "yportenv.h"

#define YAFFS_OK    1
#define YAFFS_FAIL  0

/* Give us a  Y=0x59,
 * Give us an A=0x41,
 * Give us an FF=0xff
 * Give us an S=0x53
 * And what have we got...
 */
#define YAFFS_MAGIC            0x5941ff53

/*
 * Tnodes form a tree with the tnodes in "levels"
 * Levels greater than 0 hold 8 slots which point to other tnodes.
 * Those at level 0 hold 16 slots which point to chunks in NAND.
 *
 * A maximum level of 8 thust supports files of size up to:
 *
 * 2^(3*MAX_LEVEL+4)
 *
 * Thus a max level of 8 supports files with up to 2^^28 chunks which gives
 * a maximum file size of around 512Gbytees with 2k chunks.
 */
#define YAFFS_NTNODES_LEVEL0          16
#define YAFFS_TNODES_LEVEL0_BITS      4
#define YAFFS_TNODES_LEVEL0_MASK      0xf

#define YAFFS_NTNODES_INTERNAL        (YAFFS_NTNODES_LEVEL0 / 2)
#define YAFFS_TNODES_INTERNAL_BITS    (YAFFS_TNODES_LEVEL0_BITS - 1)
#define YAFFS_TNODES_INTERNAL_MASK    0x7
#define YAFFS_TNODES_MAX_LEVEL        8
#define YAFFS_TNODES_MAX_BITS         (YAFFS_TNODES_LEVEL0_BITS + YAFFS_TNODES_INTERNAL_BITS * YAFFS_TNODES_MAX_LEVEL)
#define YAFFS_MAX_CHUNK_ID            ((1 << YAFFS_TNODES_MAX_BITS) - 1)

#define YAFFS_MAX_FILE_SIZE_32        0x7fffffff

/* Constants for YAFFS1 mode */
#define YAFFS_BYTES_PER_SPARE         16
#define YAFFS_BYTES_PER_CHUNK         512
#define YAFFS_CHUNK_SIZE_SHIFT        9
#define YAFFS_CHUNKS_PER_BLOCK        32
#define YAFFS_BYTES_PER_BLOCK         (YAFFS_CHUNKS_PER_BLOCK*YAFFS_BYTES_PER_CHUNK)

#define YAFFS_MIN_YAFFS2_CHUNK_SIZE    1024
#define YAFFS_MIN_YAFFS2_SPARE_SIZE    32



#define YAFFS_ALLOCATION_NOBJECTS      100
#define YAFFS_ALLOCATION_NTNODES       100
#define YAFFS_ALLOCATION_NLINKS        100

#define YAFFS_NOBJECT_BUCKETS          256

#define YAFFS_OBJECT_SPACE             0x40000
#define YAFFS_MAX_OBJECT_ID            (YAFFS_OBJECT_SPACE - 1)

/* Binary data version stamps */
#define YAFFS_SUMMARY_VERSION          1

#ifdef CONFIG_YAFFS_UNICODE
#define YAFFS_MAX_NAME_LENGTH          127
#define YAFFS_MAX_ALIAS_LENGTH         79
#else
#define YAFFS_MAX_NAME_LENGTH          255
#define YAFFS_MAX_ALIAS_LENGTH         159
#endif

#define YAFFS_SHORT_NAME_LENGTH        15

/* Some special object ids for pseudo objects */
#define YAFFS_OBJECTID_ROOT            1
#define YAFFS_OBJECTID_LOSTNFOUND      2
#define YAFFS_OBJECTID_UNLINKED        3
#define YAFFS_OBJECTID_DELETED         4

/* Fake object Id for summary data */
#define YAFFS_OBJECTID_SUMMARY         0x10

/* Pseudo object ids for checkpointing */
#define YAFFS_OBJECTID_CHECKPOINT_DATA    0x20
#define YAFFS_SEQUENCE_CHECKPOINT_DATA    0x21

#define YAFFS_MAX_SHORT_OP_CACHES         20

#define YAFFS_N_TEMP_BUFFERS              6

/* We limit the number attempts at sucessfully saving a chunk of data.
 * Small-page devices have 32 pages per block; large-page devices have 64.
 * Default to something in the order of 5 to 10 blocks worth of chunks.
 */
#define YAFFS_WR_ATTEMPTS                 (5*64)

/* Sequence numbers are used in YAFFS2 to determine block allocation order.
 * The range is limited slightly to help distinguish bad numbers from good.
 * This also allows us to perhaps in the future use special numbers for
 * special purposes.
 * EFFFFF00 allows the allocation of 8 blocks/second (~1Mbytes) for 15 years,
 * and is a larger number than the lifetime of a 2GB device.
 */
#define YAFFS_LOWEST_SEQUENCE_NUMBER     0x00001000
#define YAFFS_HIGHEST_SEQUENCE_NUMBER    0xefffff00

/* Special sequence number for bad block that failed to be marked bad */
#define YAFFS_SEQUENCE_BAD_BLOCK         0xffff0000

/* Chunk cache is used for short read/write operations.*/
struct yaffs_cache {
    struct yaffs_obj *object;
    int chunk_id;
    int last_use;
    int dirty;
    int n_bytes; /* Only valid if the cache is dirty */
    int locked; /* Can't push out or flush while locked. */
    u8 *data;
};

struct yaffs_cache_manager {
    struct yaffs_cache *cache;
    int n_caches;
    int cache_last_use;
    int n_temp_buffers;
};

/* yaffs1 tags structures in RAM
 * NB This uses bitfield. Bitfields should not straddle a u32 boundary
 * otherwise the structure size will get blown out.
 */

struct yaffs_tags {
    u32 chunk_id:20;
    u32 serial_number:2;
    u32 n_bytes_lsb:10;
    u32 obj_id:18;
    u32 ecc:12;
    u32 n_bytes_msb:2;
};

union yaffs_tags_union {
    struct yaffs_tags as_tags;
    u8  as_bytes[8];
    u32 as_u32[2];
};


/* Stuff used for extended tags in YAFFS2 */

enum yaffs_ecc_result {
    YAFFS_ECC_RESULT_UNKNOWN,
    YAFFS_ECC_RESULT_NO_ERROR,
    YAFFS_ECC_RESULT_FIXED,
    YAFFS_ECC_RESULT_UNFIXED
};

/*
 * Object type enum:
 * When this is stored in flash we store it as a u32 instead
 * to prevent any alignment change issues as compiler variants change.
 */

enum yaffs_obj_type {
    YAFFS_OBJECT_TYPE_UNKNOWN,
    YAFFS_OBJECT_TYPE_FILE,
    YAFFS_OBJECT_TYPE_SYMLINK,
    YAFFS_OBJECT_TYPE_DIRECTORY,
    YAFFS_OBJECT_TYPE_HARDLINK,
    YAFFS_OBJECT_TYPE_SPECIAL
};

#define YAFFS_OBJECT_TYPE_MAX YAFFS_OBJECT_TYPE_SPECIAL

struct yaffs_ext_tags {
    unsigned chunk_used; /*  Status of the chunk: used or unused，0：表示该chunk未被使用，1：表示该chunk已被使用 */
    unsigned obj_id; /* If 0 this is not used 0：表示该chunk未分配使用，非0：表示文件的obj_id信息*/
    unsigned chunk_id; /* If 0 this is a header, else a data chunk 0：表示该chunk存储的是一个文件的object header，
                        * 非0：表示该chunk存储的是data chunk，表示文件logical chunk index 可以根据这个字段计算所保留的数据在文件内的偏移
                        */
    unsigned n_bytes; /* Only valid for data chunks 表示chunk包含的数据量，单位时byte */

    /* The following stuff only has meaning when we read */
    enum yaffs_ecc_result ecc_result;
    unsigned block_bad;

    /* YAFFS 1 stuff */
    unsigned is_deleted; /* The chunk is marked deleted */
    unsigned serial_number; /* Yaffs1 2-bit serial number */

    /* YAFFS2 stuff */
    unsigned seq_number; /* The sequence number of this block 等同于擦除块的seq_number，表示擦除块被使用的先后顺序
                          * block中所有chunk的该字段是一样的，所以扫描时可以根据此信息对block机型排序
                          */

    /* Extra info if this is an object header (YAFFS2 only) */

    unsigned extra_available; /* Extra info available if not zero */
    unsigned extra_parent_id; /* The parent object */
    unsigned extra_is_shrink; /* Is it a shrink header? */
    unsigned extra_shadows; /* Does this shadow another object? */

    enum yaffs_obj_type extra_obj_type; /* What object type? 表示chunk中存储的文件类型，extra_available才使用这个信息 */

    loff_t extra_file_size; /* Length if it is a file */
    unsigned extra_equiv_id; /* Equivalent object for a hard link */
};

/* Spare structure for YAFFS1 */
struct yaffs_spare {
    u8 tb0;
    u8 tb1;
    u8 tb2;
    u8 tb3;
    u8 page_status; /* set to 0 to delete the chunk */
    u8 block_status;
    u8 tb4;
    u8 tb5;
    u8 ecc1[3];
    u8 tb6;
    u8 tb7;
    u8 ecc2[3];
};

/*Special structure for passing through to mtd */
struct yaffs_nand_spare {
    struct yaffs_spare spare;
    int eccres1;
    int eccres2;
};

/* Block data in RAM */

enum yaffs_block_state {
    YAFFS_BLOCK_STATE_UNKNOWN = 0,

    YAFFS_BLOCK_STATE_SCANNING,
    /* Being scanned */

    YAFFS_BLOCK_STATE_NEEDS_SCAN,
    /* The block might have something on it (ie it is allocating or full,
     * perhaps empty) but it needs to be scanned to determine its true
     * state.
     * This state is only valid during scanning.
     * NB We tolerate empty because the pre-scanner might be incapable of
     * deciding
     * However, if this state is returned on a YAFFS2 device,
     * then we expect a sequence number
     */

    YAFFS_BLOCK_STATE_EMPTY,
    /* This block is empty */

    YAFFS_BLOCK_STATE_ALLOCATING,
    /* This block is partially allocated.
     * At least one page holds valid data.
     * This is the one currently being used for page
     * allocation. Should never be more than one of these.
     * If a block is only partially allocated at mount it is treated as
     * full.
     */

    YAFFS_BLOCK_STATE_FULL,
    /* All the pages in this block have been allocated.
     * If a block was only partially allocated when mounted we treat
     * it as fully allocated.
     */

    YAFFS_BLOCK_STATE_DIRTY,
    /* The block was full and now all chunks have been deleted.
     * Erase me, reuse me.
     */

    YAFFS_BLOCK_STATE_CHECKPOINT,
    /* This block is assigned to holding checkpoint data. */

    YAFFS_BLOCK_STATE_COLLECTING,
    /* This block is being garbage collected */

    YAFFS_BLOCK_STATE_DEAD
        /* This block has failed and is not in use */
};

#define    YAFFS_NUMBER_OF_BLOCK_STATES (YAFFS_BLOCK_STATE_DEAD + 1)

struct yaffs_block_info {

    s32 soft_del_pages:10; /* number of soft deleted pages 在YAFFS2包含两种类型的删除，delete和soft delete。
                            * 前者用于文件内容的更新，比如修改文件中的部分内容，这时YAFFS2会分配新的chunk，将修改后的
                            * 内容写入新的chunk中，原来chunk的内容自然就没用了，所有将pages_in_use减1，并修改chunk_bits.
                            * 后者用于文件删除，YAFFS2在删除文件时，只有删除该文件在内存中的一些描述结构，而被删除文件所占
                            * 用的chunk不会立即释放，不会删除文件内容，在后续的文件系统操作中一般也不会把这些chunk分配出去，
                            * 直到系统进行系统垃圾收集时才会选择释放这些chunk。soft_del_pages就表示soft delete的chunk数目
                            */
    s32 pages_in_use:10; /* number of pages in use 该擦除块中被使用的chunk数目，包括已经被soft delete的chunk */
    u32 block_state:4; /* One of the above block states. NB use unsigned because enum is sometimes an int 
                        * 该擦除快的状态，比如，YAFFS_BLOCK_STATE_FULL表该擦除块中所有的chunk已经被分配完，
                        * YAFFS_BLOCK_STATE_DIRTY表示该擦除块中所有的chunk已经被delete可以被擦除，YAFFS_BLOCK_STATE_EMPTY
                        * 表示空闲的擦除块
                        */
    u32 needs_retiring:1; /* Data has failed on this block, need to get valid data off and retire
                           * 标记位，chunk_error_strikes次超过3次以上就会置位，该擦除快需要重新回收块
                           */
    u32 skip_erased_check:1; /* Skip the erased check on this block，标记位，置0时需要对擦除块的进行检测，一般额只会检测
                             * 该擦除的第一个chunk，置1时跳过对擦除块的检测，可以通过CONFIG_YAFFS_ALWAYS_CHECK_CHUNK_ERASED设置 
                             */
    u32 gc_prioritise:1; /* An ECC check or blank check has failed. Block should be prioritised for GC 
                          * 标志位，该块发生ECC校验错误或check失败，需要在垃圾回收时优先擦除
                          */
    u32 chunk_error_strikes:3; /* How many times we've had ecc etc ilures on this block and tried to reuse it
                                * 发生ECC校验错误的次数
                                */
    u32 has_summary:1;    /* The block has a summary */
    u32 has_shrink_hdr:1;    /* This block has at least one shrink header 标志位，置0时表示该擦除快上存储文件被没发生过
                              * 截断truncate，即文件大小没有发生过变化resize，否则必须在文件的objecttheader标识，同时该object
                              * head所在擦除块上页标识，即has_shrink_hdr置1
                              */
    u32 seq_number;    /* block sequence number for yaffs2，序列号，表示擦除块被使用使用的先后顺序，序号越小越早被使用，
                        * 在挂载时起到非常重要的作用
                        */
};

union yaffs_block_info_union {
    struct yaffs_block_info bi;
    u32 as_u32[2];
};

/* -------------------------- Object structure -------------------------------*/
/* This is the object structure as stored on NAND */

struct yaffs_obj_hdr {
    u32 type;  /* enum yaffs_obj_type 关联文件类型，包括普通文件、目录还是链接文件等  */

    /* Apply to everything  */
    u32 parent_obj_id;
    u16 sum_no_longer_used; /* checksum of name. No longer used */
    YCHAR name[YAFFS_MAX_NAME_LENGTH + 1];

    /* The following apply to all object types except for hard links */
    u32 yst_mode; /* protection */

    u32 yst_uid;
    u32 yst_gid;
    u32 yst_atime;
    u32 yst_mtime;
    u32 yst_ctime;

    /* File size  applies to files only */
    u32 file_size_low;

    /* Equivalent object id applies to hard links only. */
    int equiv_id;

    /* Alias is for symlinks only. */
    YCHAR alias[YAFFS_MAX_ALIAS_LENGTH + 1];

    u32 yst_rdev; /* stuff for block and char devices (major/min) */

    /*
     * WinCE times are no longer just used to store WinCE times.
     * They are also used to store 64-bit times.
     * We actually store and read the times in both places and use
     * the best we can.
     */
    u32 win_ctime[2];
    u32 win_atime[2];
    u32 win_mtime[2];

    u32 inband_shadowed_obj_id;
    u32 inband_is_shrink;

    u32 file_size_high;
    u32 reserved[1];
    int shadows_obj; /* This object header shadows the
                specified object if > 0 */

    /* is_shrink applies to object headers written when wemake a hole. */
    u32 is_shrink; /* 所关联的文件发生过resize，如果file发生过resize，必须在file的object head上表示出
                    这一点，同时必须在obj header所在block上也标识处出这点 */

};

/*--------------------------- Tnode -------------------------- */

struct yaffs_tnode {
    struct yaffs_tnode *internal[YAFFS_NTNODES_INTERNAL];
};

/*------------------------  Object -----------------------------*/
/* An object can be one of:
 * - a directory (no data, has children links
 * - a regular file (data.... not prunes :->).
 * - a symlink [symbolic link] (the alias).
 * - a hard link
 */

/* The file variant has three file sizes:
 *  - file_size : size of file as written into Yaffs - including data in cache.
 *  - stored_size - size of file as stored on media.
 *  - shrink_size - size of file that has been shrunk back to.
 *
 * The stored_size and file_size might be different because the data written
 * into the cache will increase the file_size but the stored_size will only
 * change when the data is actually stored.
 *
 */
struct yaffs_file_var {
    loff_t file_size;
    loff_t stored_size;
    loff_t shrink_size;
    int top_level;
    struct yaffs_tnode *top;
};

struct yaffs_dir_var {
    struct list_head children; /* list of child links */
    struct list_head dirty; /* Entry for list of dirty directories */
};

struct yaffs_symlink_var {
    YCHAR *alias;
};

struct yaffs_hardlink_var {
    struct yaffs_obj *equiv_obj;
    u32 equiv_id;
};

union yaffs_obj_var {
    struct yaffs_file_var file_variant;
    struct yaffs_dir_var dir_variant;
    struct yaffs_symlink_var symlink_variant;
    struct yaffs_hardlink_var hardlink_variant;
};

struct yaffs_obj {
    u8 deleted:1; /* This should only apply to unlinked files. */
    u8 soft_del:1; /* it has also been soft deleted */
    u8 unlinked:1; /* An unlinked file.*/
    u8 fake:1; /* A fake object has no presence on NAND. */
    u8 rename_allowed:1; /* Some objects cannot be renamed. */
    u8 unlink_allowed:1;
    u8 dirty:1; /* the object needs to be written to flash  文件被修改时置1*/
    u8 valid:1; /* When the file system is being loaded up, this object might be created before the data
                 * is available ie. file data chunks encountered before the header.
                 * 文件oject header写入的时候置1，没有object header，只存在内存中，是root、lost+found、delete与unlink
                 */
    u8 lazy_loaded:1; /* This object has been lazy loaded and is missing some detail */

    u8 defered_free:1; /* Object is removed from NAND, but is still in the inode cache. Free of object is defered.
                 * until the inode is released.
                 */
    u8 being_created:1; /* This object is still being created so skip some verification checks. */
    u8 is_shadowed:1; /* This object is shadowed on the way to being renamed. */
    u8 xattr_known:1; /* We know if this has object has xattribs or not. */
    u8 has_xattr:1; /* This object has xattribs. Only valid if xattr_known. */

    u8 serial; /* serial number of chunk in NAND.*/
    u16 sum; /* sum of the name to speed searching */

    struct yaffs_dev *my_dev; /* The device I'm on */

    struct list_head hash_link; /* list of objects in hash bucket 通过该字段把文件对象挂载到散列表中 */

    struct list_head hard_links; /* hard linked object chain*/

    /* directory structure stuff */
    /* also used for linking up the free list */
    struct yaffs_obj *parent; /* 指向父目录的yaffs_obj对象 */
    struct list_head siblings; /* 在同一个目录的yaffs_obj对象通过该字段组成双向循环链表 */

    /* Where's my object header in NAND? */
    int hdr_chunk; /* 每个文件在flash都有一个object header，存储着该文件的大小、所有者、创建者修改时间等信息
                    * hdr_chunk就是该文件头在flash上的chunk中写入一个新的object header */

    int n_data_chunks; /* Number of data chunks for this file. */

    u32 obj_id; /* the object id value 每一个文件系统对象都被赋予一个唯一的编号，作为对象标识，也用于将该对象挂入
                   一个散列表，加速对象的搜索速度 */
    u32 yst_mode;

    YCHAR short_name[YAFFS_SHORT_NAME_LENGTH + 1];

#ifdef CONFIG_YAFFS_WINCE
    //these are always 64 bits
    u32 win_ctime[2];
    u32 win_mtime[2];
    u32 win_atime[2];
#else
    //these can be 32 or 64 bits
    YTIME_T yst_uid;
    YTIME_T yst_gid;
    YTIME_T yst_atime;
    YTIME_T yst_mtime;
    YTIME_T yst_ctime;
#endif

    u32 yst_rdev;

    void *my_inode;

    u32 variant_type; /* enum yaffs_object_type 表示对象的类型，是目录文件、普通文件还是链接文件 */

    union yaffs_obj_var variant;

};

struct yaffs_obj_bucket {
    struct list_head list;
    int count;
};


/*--------------------- Temporary buffers ----------------
 *
 * These are chunk-sized working buffers. Each device has a few.
 */

struct yaffs_buffer {
    u8 *buffer;
    int in_use;
};

/*----------------- Device ---------------------------------*/

struct yaffs_param {
    const YCHAR *name;

    /*
     * Entry parameters set up way early. Yaffs sets up the rest.
     * The structure should be zeroed out before use so that unused
     * and default values are zero.
     */

    int inband_tags; /* Use unband tags 标志位，默认为0，即采用OOB（out of band）方式存储tags，可以通过挂载时指定inband-tags选项改变默认值 */
    u32 total_bytes_per_chunk; /* Should be >= 512, does not need to be a power of 2 每个chunk总的字节数 */
    u32 chunks_per_block; /* does not need to be a power of 2 每个block总的chunk数 */
    u32 spare_bytes_per_chunk; /* spare area size 每个chunk包含OOB块的字节数 */
    u32 start_block; /* Start block we're allowed to use 第一个可以使用的block */
    u32 end_block; /* End block we're allowed to use 最后一个可以使用额block */
    u32 n_reserved_blocks; /* Tuneable so that we can reduce reserved blocks on NOR and RAM. 为GC保留的block阈值*/
    u32 n_caches; /* If == 0, then short op caching is disabled, else the number of short op caches. 
                     缓冲区的个数，YAFFS为了减少数据的碎片以及提高性能为每个文件写入提供cache */
                         */
    int cache_bypass_aligned; /* If non-zero then bypass the cache for aligned writes. */
    int use_nand_ecc; /* Flag to decide whether or not to use NAND driver ECC on data (yaffs1) */
    int tags_9bytes; /* Use 9 byte tags */
    int no_tags_ecc; /* Flag to decide whether or not to do ECC on packed tags (yaffs2) 
                      * 标志位，默认为0，即tags中包括ECC纠错信息，可以通过内核配置改变默认值，CONFIG_YAFFS_DISABLE_TAGS_ECC
                      */
    int is_yaffs2; /* Use yaffs2 mode on this device 标志位默认为0，即YAFFS，在挂载的过程中会根据识别的mtd->writesize自动转换成YAFFS */
    int empty_lost_n_found; /* Auto-empty lost+found directory on mount */
    int refresh_period; /* How often to check for a block refresh 刷新周期,刷新目的的主要找出最旧的处于YAFFS_BLOCK_STATE_FULL状态的block，供GC作为gc_block使用 */

    /* Checkpoint control. Can be set before or after initialisation */
    u8 skip_checkpt_rd; /* 标志位，默认为0，支持读取checkpoint，提供挂载速度的一个功能可以通过挂载时指定挂载选项 */
    u8 skip_checkpt_wr; /* 标志位，默认为0，支持写入checkpoint,提高挂载速度的一个功能可以通过挂载时指定挂载选项
                         * no-checkpoint-read、no-checkpoint修改默认值 */

    int enable_xattr; /* Enable xattribs */

    int max_objects; /* Set to limit the number of objects created. 0 = no limit. */

    int hide_lost_n_found;  /* Set non-zero to hide the lost-n-found dir. */

    int stored_endian; /* 0=cpu endian, 1=little endian, 2=big endian */

    /* The remove_obj_fn function must be supplied by OS flavours that
     * need it.
     * yaffs direct uses it to implement the faster readdir.
     * Linux uses it to protect the directory during unlocking.
     */
    void (*remove_obj_fn) (struct yaffs_obj *obj);

    /* Callback to mark the superblock dirty */
    void (*sb_dirty_fn) (struct yaffs_dev *dev);

    /*  Callback to control garbage collection. */
    unsigned (*gc_control_fn) (struct yaffs_dev *dev);

    /* Debug control flags. Don't use unless you know what you're doing */
    int use_header_file_size; /* Flag to determine if we should use file sizes from the header */
    int disable_lazy_load; /* Disable lazy loading on this device */
    int wide_tnodes_disabled; /* Set to disable wide tnodes 标志位，默认为0，采用动态位宽，通过内核配置修改采用静态位宽CONFIG_YAFFS_DISABLE_WIDTH_TNODES */
    int disable_soft_del; /* yaffs 1 only: Set to disable the use of * softdeletion. */

    int defered_dir_update; /* Set to defer directory updates */

#ifdef CONFIG_YAFFS_AUTO_UNICODE
    int auto_unicode;
#endif
    int always_check_erased; /* Force chunk erased check always on */

    int disable_summary;
    int disable_bad_block_marking;

};

struct yaffs_driver {
    int (*drv_write_chunk_fn) (struct yaffs_dev *dev, int nand_chunk, /* 在挂载的文件系统时候 被初始化，NAND flash写入接口函数 */
                   const u8 *data, int data_len,
                   const u8 *oob, int oob_len);
    int (*drv_read_chunk_fn) (struct yaffs_dev *dev, int nand_chunk, /* 在挂载的文件系统时候 被初始化，NAND flash读取接口函数 */
                   u8 *data, int data_len,
                   u8 *oob, int oob_len,
                   enum yaffs_ecc_result *ecc_result);
    int (*drv_erase_fn) (struct yaffs_dev *dev, int block_no); /* 在挂载的文件系统时候 被初始化，NAND flash擦除block接口函数 */
    int (*drv_mark_bad_fn) (struct yaffs_dev *dev, int block_no);
    int (*drv_check_bad_fn) (struct yaffs_dev *dev, int block_no);
    int (*drv_initialise_fn) (struct yaffs_dev *dev);
    int (*drv_deinitialise_fn) (struct yaffs_dev *dev);
};

struct yaffs_tags_handler {
    int (*write_chunk_tags_fn) (struct yaffs_dev *dev, int nand_chunk, const u8 *data, 
                                const struct yaffs_ext_tags *tags); /* 在挂载的文件系统的时候会被初始化 */
    int (*read_chunk_tags_fn) (struct yaffs_dev *dev,
                   int nand_chunk, u8 *data,
                   struct yaffs_ext_tags *tags);

    int (*query_block_fn) (struct yaffs_dev *dev, int block_no,
                   enum yaffs_block_state *state,
                   u32 *seq_number);
    int (*mark_bad_fn) (struct yaffs_dev *dev, int block_no);
};

struct yaffs_dev {
    struct yaffs_param param;
    struct yaffs_driver drv;
    struct yaffs_tags_handler tagger;

    /* Context storage. Holds extra OS specific data for this device */

    void *os_context; /* 指向yaffs_linux_context结构体，该结构体YAFFS运行环境 */
    void *driver_context; /* 指向mtd_info结构指针，mtd_info是MTD子系统核心的数据结构，主要是对底层硬件驱动进行封装 */

    struct list_head dev_list;

    int ll_init;
    /* Runtime parameters. Set up by YAFFS. */
    u32 data_bytes_per_chunk; /* 每个chunk总字节数，和前面提到的total_bytes_per_chunk一样 */

    /* Non-wide tnode stuff */
    u16 chunk_grp_bits; /* Number of bits that need to be resolved if the tnodes are not wide enough.
                         * 采用静态位宽超时tmode_width宽度之后的位数，采用动态参数恒为0
                         */
    u16 chunk_grp_size; /* == 2^^chunk_grp_bits，由chunk_grp_bits转化而来的大小 */

    struct yaffs_tnode *tn_swap_buffer;

    /* Stuff to support wide tnodes */
    u32 tnode_width; /* 采用静态位宽默认时16，采用动态位宽是由整个nand flash 中chunk数目计算得到 */
    u32 tnode_mask; /* 位宽mask，主要用于快速获取chunk id号 */
    u32 tnode_size; /* YAFFS_NTNIODES_LEVEL0节点所占用内存大小，单位：byte */

    /* Stuff for figuring out file offset to chunk conversions */
    u32 chunk_shift; /* Shift value 主要用来计算logical chunk index以及logical chunk offset */
    u32 chunk_div; /* Divisor after shifting: 1 for 2^n sizes 作用同chunk_shift, 主要用于chunk大小不是2次幂的情况 */
    u32 chunk_mask; /* Mask to use for power-of-2 case 作用同chunk_shift，组合起来计算logical chunk offset */

    int is_mounted; /* 标志位，文件系统被挂载时被置位 */
    int read_only;
    int is_checkpointed;
    int swap_endian; /* Stored endian needs endian swap. */

    /* Stuff to support block offsetting to support start block zero */
    u32 internal_start_block;
    u32 internal_end_block;
    int block_offset;
    int chunk_offset;

    /* Runtime checkpointing stuff，checkpointing是为了提高挂载速度而引入的功能，作用同JFFS的EBS，以空间换时间，
       卸载时通过在NAND flash上保存文件系统超级块快照，挂载时获取快照信息可以快速还原系统状态 */
    int checkpt_page_seq; /* running sequence number of checkpt pages 控制checkpoint的写入或读出时的chunk序列号 */
    int checkpt_byte_count; /* 写入或读出checkpoint信息的字节数 */
    int checkpt_byte_offs; /* checkpt_buffer缓冲区的偏移量 */
    u8 *checkpt_buffer; /* 写入或读出checkpoint的缓存区，大小为data_byte_per_chunk字节 */
    int checkpt_open_write; /* 标志位，yaffs_check_open时传入的，决定checkpoint读写属性 */
    u32 blocks_in_checkpt; /* checkpoint使用的block的数量 */
    int checkpt_cur_chunk; /* 当前block已经使用掉的chunk数量 */
    int checkpt_cur_block; /* 当前正在使用的block编码 */
    int checkpt_next_block; /* 当前正在使用的下一个block编码 */
    int *checkpt_block_list; /* 在读checkpoint时使用，该数组中保留使用的block编码，checkpoint信息读取完毕，根据这个
                              * 个数组保存的block编号，更改block的状态为YAFFS_BLOCK_TATE_CHECKPOINT，以便卸载时
                              * 检测这样状态的block最大数量
                              */
    u32 checkpt_max_blocks; /* checkpoint能使的block最大数量 */
    u32 checkpt_sum;
    u32 checkpt_xor;
    int checkpoint_blocks_required; /* Number of blocks needed to store current checkpoint set 
                                     * checkpoint信息所需的block数量，看该字段的计算大致就知道checkpoint
                                     * 需要保存文件系统的那些信息
                                     */
    /* Block Info */
    struct yaffs_block_info *block_info; /* 数组每一项表示一块擦除的统计信息，由yaffs_block_info表示，该信息在运行时只存在
                                          * 内存中，当YAFFS2被卸载时，该数据当作chekpoint信息被记录被记录下，在下一次挂载时
                                          * 被读出并恢复
                                          */
    u8 *chunk_bits; /* bitmap of chunks in use 指向擦除块位图的指针，每一位对应一个chunk，置0表示没有被使用，置表示使用中 */
    u8 block_info_alt:1; /* allocated using alternative alloc 标志位，采用kmalloc分配block_info所使用的内存则置0，使用vmalloc分配置1 */
    u8 chunk_bits_alt:1; /* allocated using alternative alloc 标志位，采用kmalloc分配chunk_bits所使用的内存则置0，使用vmalloc分配置1 */
    int chunk_bit_stride; /* Number of bytes of chunk_bits per block. Must be consistent with chunks_per_block.
                           * 位宽，擦除块中每个chunk占用一位，总的位宽应等于chunk_per_block，但要按byte对其
                           */

    int n_erased_blocks; /* 空闲擦除的数目 */
    int alloc_block; /* Current block being allocated off 当前正在被写入的擦除块在block_info指针数组的下标 */
    u32 alloc_page; /* 当前正在被写入的擦除块中chunk的顺序号 */
    int alloc_block_finder; /* Used to search for next allocation block 记录下一个被选择写入的擦除块在block_info指针数组的下表 */

    /* Object and Tnode memory management */
    void *allocator; /* 指向struct yaffs_allocation结构的指针，YAFFS2文件系统实现的一个内存分配器，主要为了struck
                      * yaffs_allocator、struct yaffs_tnode分配高速缓存替代原有的sab、struct yaffs_allocator
                      */
    int n_obj; /* 文件系统中已经分配的struct yaffs_obj的数量，计算checkpoint信息时使用 */
    int n_tnodes; /* 文件系统中已经分配的struct yaffs_tnode的数量，计算checkpoint信息时使用 */

    int n_hardlinks;

    struct yaffs_obj_bucket obj_bucket[YAFFS_NOBJECT_BUCKETS]; /* struct yaffs_obj对象的散列表，以obj_id为键，便于文件查找和操作 */
    u32 bucket_finder; /* 散列表中最短链表的索引 */

    int n_free_chunks; /* flash中空闲的chunk数量 */

    /* Garbage collection control */
    u32 *gc_cleanup_list;    /* objects to delete at the end of a GC. 数组，保存垃圾回收时可以删除的yaffs_obj对象obj_id */
    u32 n_clean_ups; /* 上面提到的数组下标 */

    unsigned has_pending_prioritised_gc; /* We think this device might have pending prioritised gcs 标志位，设备存在优先回收的擦除快 */
    unsigned gc_disable; /* 标志位，置1禁止垃圾回收，置0使能垃圾回收，主要用于垃圾回收时的同步，防止垃圾回收的重入，进入垃圾流程置1，退出时置0 */
    unsigned gc_block_finder; /* 存储可以进行垃圾回收擦除块的编号 */
    unsigned gc_dirtiest; /* 存储最脏的擦除块的编号 */
    unsigned gc_pages_in_use; /* 被选中垃圾回收的擦除块有效收据使用page数目不能超过一定阈值，否则代价太大，需要把有效数据搬移到空闲擦除的page中 */
    unsigned gc_not_done; /* 跳过垃圾回收的次数 */
    unsigned gc_block; /* 正在被垃圾回收的擦除块 */
    unsigned gc_chunk; /* 垃圾回收时会判断每个chunk是否有效，gc_chunk表示正在被检查的那个chunk，有效的话需要把该chunk的数据搬移到其他空闲擦除块的chunk上 */
    unsigned gc_skip;
    struct yaffs_summary_tags *gc_sum_tags;

    /* Special directories */
    struct yaffs_obj *root_dir;
    struct yaffs_obj *lost_n_found;

    int buffered_block; /* Which block is buffered here? */
    int doing_buffered_block_rewrite;

    struct yaffs_cache_manager cache_mgr;

    /* Stuff for background deletion and unlinked files. */
    struct yaffs_obj *unlinked_dir; /* Directory where unlinked and deleted files live. */
    struct yaffs_obj *del_dir; /* Directory where deleted objects are sent to disappear. */
    struct yaffs_obj *unlinked_deletion; /* Current file being background deleted. */
    int n_deleted_files; /* Count of files awaiting deletion; */
    int n_unlinked_files; /* Count of unlinked files. */
    int n_bg_deletions; /* Count of background deletions. */

    /* Temporary buffer management */
    struct yaffs_buffer temp_buffer[YAFFS_N_TEMP_BUFFERS]; /* 一个临时的buffer，buffer大小total_bytes_per_chunk, 主要用来临时存放从chunk中读出的数据 */
    int max_temp;
    int temp_in_use;
    int unmanaged_buffer_allocs;
    int unmanaged_buffer_deallocs;

    /* yaffs2 runtime stuff */
    unsigned seq_number; /* Sequence number of currently allocating block 当被用作序列号时，sequnce number表示擦除块被使用的前后顺序，越小则表示被使用的越早 */
    unsigned oldest_dirty_seq; /* 最小的seq_number号也是最老的seq_number号 */
    unsigned oldest_dirty_block; /* 最小的seq_number号对应的擦除块编号 */

    /* Block refreshing */
    int refresh_skip; /* A skip down counter.Refresh happens when this gets to zero. */

    /* Dirty directory handling */
    struct list_head dirty_dirs; /* List of dirty directories */

    /* Summary */
    int chunks_per_summary;
    struct yaffs_summary_tags *sum_tags;

    /* Statistics */
    u32 n_page_writes;
    u32 n_page_reads;
    u32 n_erasures;
    u32 n_bad_queries;
    u32 n_bad_markings;
    u32 n_erase_failures;
    u32 n_gc_copies;
    u32 all_gcs;
    u32 passive_gc_count;
    u32 oldest_dirty_gc_count;
    u32 n_gc_blocks;
    u32 bg_gcs;
    u32 n_retried_writes;
    u32 n_retired_blocks;
    u32 n_ecc_fixed;
    u32 n_ecc_unfixed;
    u32 n_tags_ecc_fixed;
    u32 n_tags_ecc_unfixed;
    u32 n_deletions;
    u32 n_unmarked_deletions;
    u32 refresh_count;
    u32 cache_hits;
    u32 tags_used;
    u32 summary_used;

};

/*
 * Checkpointing definitions.
 */

#define YAFFS_CHECKPOINT_VERSION    8

/* yaffs_checkpt_obj holds the definition of an object as dumped
 * by checkpointing.
 */


/*  Checkpint object bits in bitfield: offset, length */
#define CHECKPOINT_VARIANT_BITS           0, 3
#define CHECKPOINT_DELETED_BITS           3, 1
#define CHECKPOINT_SOFT_DEL_BITS          4, 1
#define CHECKPOINT_UNLINKED_BITS          5, 1
#define CHECKPOINT_FAKE_BITS              6, 1
#define CHECKPOINT_RENAME_ALLOWED_BITS    7, 1
#define CHECKPOINT_UNLINK_ALLOWED_BITS    8, 1
#define CHECKPOINT_SERIAL_BITS            9, 8

struct yaffs_checkpt_obj {
    int struct_type;
    u32 obj_id;
    u32 parent_id;
    int hdr_chunk;
    u32 bit_field;
    int n_data_chunks;
    loff_t size_or_equiv_obj;
};

/* The CheckpointDevice structure holds the device information that changes
 *at runtime and must be preserved over unmount/mount cycles.
 */
struct yaffs_checkpt_dev {
    int struct_type;
    int n_erased_blocks;
    int alloc_block; /* Current block being allocated off */
    u32 alloc_page;
    int n_free_chunks;

    int n_deleted_files; /* Count of files awaiting deletion; */
    int n_unlinked_files; /* Count of unlinked files. */
    int n_bg_deletions; /* Count of background deletions. */

    /* yaffs2 runtime stuff */
    unsigned seq_number; /* Sequence number of currently allocating block */

};

struct yaffs_checkpt_validity {
    int struct_type;
    u32 magic;
    u32 version;
    u32 head;
};

struct yaffs_shadow_fixer {
    int obj_id;
    int shadowed_id;
    struct yaffs_shadow_fixer *next;
};

/* Structure for doing xattr modifications */
struct yaffs_xattr_mod {
    int set; /* If 0 then this is a deletion */
    const YCHAR *name;
    const void *data;
    int size;
    int flags;
    int result;
};

/*----------------------- YAFFS Functions -----------------------*/

int yaffs_guts_initialise(struct yaffs_dev *dev);
void yaffs_deinitialise(struct yaffs_dev *dev);
void yaffs_guts_cleanup(struct yaffs_dev *dev);

int yaffs_get_n_free_chunks(struct yaffs_dev *dev);

int yaffs_rename_obj(struct yaffs_obj *old_dir, const YCHAR * old_name,
             struct yaffs_obj *new_dir, const YCHAR * new_name);

int yaffs_unlink_obj(struct yaffs_obj *obj);

int yaffs_unlinker(struct yaffs_obj *dir, const YCHAR * name);
int yaffs_del_obj(struct yaffs_obj *obj);
struct yaffs_obj *yaffs_retype_obj(struct yaffs_obj *obj, enum yaffs_obj_type type);


int yaffs_get_obj_name(struct yaffs_obj *obj, YCHAR * name, int buffer_size);
loff_t yaffs_get_obj_length(struct yaffs_obj *obj);
int yaffs_get_obj_inode(struct yaffs_obj *obj);
unsigned yaffs_get_obj_type(struct yaffs_obj *obj);
int yaffs_get_obj_link_count(struct yaffs_obj *obj);

/* File operations */
int yaffs_file_rd(struct yaffs_obj *obj, u8 * buffer, loff_t offset, int n_bytes);
int yaffs_wr_file(struct yaffs_obj *obj, const u8 * buffer, loff_t offset, int n_bytes, int write_trhrough);
int yaffs_resize_file(struct yaffs_obj *obj, loff_t new_size);

struct yaffs_obj *yaffs_create_file(struct yaffs_obj *parent, const YCHAR *name, u32 mode, u32 uid, u32 gid);

int yaffs_flush_file(struct yaffs_obj *in, int update_time, int data_sync, int discard_cache);

/* Flushing and checkpointing */
void yaffs_flush_whole_cache(struct yaffs_dev *dev, int discard);

int yaffs_checkpoint_save(struct yaffs_dev *dev);
int yaffs_checkpoint_restore(struct yaffs_dev *dev);

/* Directory operations */
struct yaffs_obj *yaffs_create_dir(struct yaffs_obj *parent, const YCHAR *name, u32 mode, u32 uid, u32 gid);
struct yaffs_obj *yaffs_find_by_name(struct yaffs_obj *the_dir, const YCHAR *name);
struct yaffs_obj *yaffs_find_by_number(struct yaffs_dev *dev, u32 number);

/* Link operations */
struct yaffs_obj *yaffs_link_obj(struct yaffs_obj *parent, const YCHAR *name,
                 struct yaffs_obj *equiv_obj);

struct yaffs_obj *yaffs_get_equivalent_obj(struct yaffs_obj *obj);

/* Symlink operations */
struct yaffs_obj *yaffs_create_symlink(struct yaffs_obj *parent, const YCHAR *name, u32 mode, u32 uid, u32 gid, const YCHAR *alias);
YCHAR *yaffs_get_symlink_alias(struct yaffs_obj *obj);

/* Special inodes (fifos, sockets and devices) */
struct yaffs_obj *yaffs_create_special(struct yaffs_obj *parent,
                       const YCHAR *name, u32 mode, u32 uid,
                       u32 gid, u32 rdev);

int yaffs_set_xattrib(struct yaffs_obj *obj, const YCHAR *name,
              const void *value, int size, int flags);
int yaffs_get_xattrib(struct yaffs_obj *obj, const YCHAR *name, void *value,
              int size);
int yaffs_list_xattrib(struct yaffs_obj *obj, char *buffer, int size);
int yaffs_remove_xattrib(struct yaffs_obj *obj, const YCHAR *name);

/* Special directories */
struct yaffs_obj *yaffs_root(struct yaffs_dev *dev);
struct yaffs_obj *yaffs_lost_n_found(struct yaffs_dev *dev);

void yaffs_handle_defered_free(struct yaffs_obj *obj);

void yaffs_update_dirty_dirs(struct yaffs_dev *dev);

int yaffs_bg_gc(struct yaffs_dev *dev, unsigned urgency);

/* Debug dump  */
int yaffs_dump_obj(struct yaffs_obj *obj);

void yaffs_guts_test(struct yaffs_dev *dev);
int yaffs_guts_ll_init(struct yaffs_dev *dev);


/* A few useful functions to be used within the core files*/
void yaffs_chunk_del(struct yaffs_dev *dev, int chunk_id, int mark_flash, int lyn);
int yaffs_check_ff(u8 *buffer, int n_bytes);
void yaffs_handle_chunk_error(struct yaffs_dev *dev, struct yaffs_block_info *bi);

u8 *yaffs_get_temp_buffer(struct yaffs_dev *dev);
void yaffs_release_temp_buffer(struct yaffs_dev *dev, u8 *buffer);

struct yaffs_obj *yaffs_find_or_create_by_number(struct yaffs_dev *dev, int number, enum yaffs_obj_type type);
int yaffs_put_chunk_in_file(struct yaffs_obj *in, int inode_chunk, int nand_chunk, int in_scan);
void yaffs_set_obj_name(struct yaffs_obj *obj, const YCHAR *name);
void yaffs_set_obj_name_from_oh(struct yaffs_obj *obj, const struct yaffs_obj_hdr *oh);
void yaffs_add_obj_to_dir(struct yaffs_obj *directory, struct yaffs_obj *obj);
YCHAR *yaffs_clone_str(const YCHAR *str);
void yaffs_link_fixup(struct yaffs_dev *dev, struct list_head *hard_list);
void yaffs_block_became_dirty(struct yaffs_dev *dev, int block_no);
int yaffs_update_oh(struct yaffs_obj *in, const YCHAR *name, int force, int is_shrink, int shadows,
            struct yaffs_xattr_mod *xop);
void yaffs_handle_shadowed_obj(struct yaffs_dev *dev, int obj_id, int backward_scanning);
int yaffs_check_alloc_available(struct yaffs_dev *dev, int n_chunks);
struct yaffs_tnode *yaffs_get_tnode(struct yaffs_dev *dev);
struct yaffs_tnode *yaffs_add_find_tnode_0(struct yaffs_dev *dev, struct yaffs_file_var *file_struct,
                       u32 chunk_id, struct yaffs_tnode *passed_tn);

int yaffs_do_file_wr(struct yaffs_obj *in, const u8 *buffer, loff_t offset, int n_bytes, int write_trhrough);
void yaffs_resize_file_down(struct yaffs_obj *obj, loff_t new_size);
void yaffs_skip_rest_of_block(struct yaffs_dev *dev);

int yaffs_count_free_chunks(struct yaffs_dev *dev);

struct yaffs_tnode *yaffs_find_tnode_0(struct yaffs_dev *dev, struct yaffs_file_var *file_struct, u32 chunk_id);

u32 yaffs_get_group_base(struct yaffs_dev *dev, struct yaffs_tnode *tn, unsigned pos);

int yaffs_is_non_empty_dir(struct yaffs_obj *obj);

int yaffs_guts_format_dev(struct yaffs_dev *dev);

void yaffs_addr_to_chunk(struct yaffs_dev *dev, loff_t addr, int *chunk_out, u32 *offset_out);
/*
 * Marshalling functions to get loff_t file sizes into and out of
 * object headers.
 */
void yaffs_oh_size_load(struct yaffs_dev *dev, struct yaffs_obj_hdr *oh, loff_t fsize, int do_endian);
loff_t yaffs_oh_to_size(struct yaffs_dev *dev, struct yaffs_obj_hdr *oh, int do_endian);
loff_t yaffs_max_file_size(struct yaffs_dev *dev);


/* yaffs_wr_data_obj needs to be exposed to allow the cache to access it. */
int yaffs_wr_data_obj(struct yaffs_obj *in, int inode_chunk, const u8 *buffer, int n_bytes, int use_reserve);

/*
 * Debug function to count number of blocks in each state
 * NB Needs to be called with correct number of integers
 */

void yaffs_count_blocks_by_state(struct yaffs_dev *dev, int bs[10]);

int yaffs_find_chunk_in_file(struct yaffs_obj *in, int inode_chunk, struct yaffs_ext_tags *tags);

/*
 *Time marshalling functions
 */

YTIME_T yaffs_oh_ctime_fetch(struct yaffs_obj_hdr *oh);
YTIME_T yaffs_oh_mtime_fetch(struct yaffs_obj_hdr *oh);
YTIME_T yaffs_oh_atime_fetch(struct yaffs_obj_hdr *oh);

void yaffs_oh_ctime_load(struct yaffs_obj *obj, struct yaffs_obj_hdr *oh);
void yaffs_oh_mtime_load(struct yaffs_obj *obj, struct yaffs_obj_hdr *oh);
void yaffs_oh_atime_load(struct yaffs_obj *obj, struct yaffs_obj_hdr *oh);

/*
 * Define LOFF_T_32_BIT if a 32-bit LOFF_T is being used.
 * Not serious if you get this wrong - you might just get some warnings.
*/

#ifdef  LOFF_T_32_BIT
#define FSIZE_LOW(fsize) (fsize)
#define FSIZE_HIGH(fsize) 0
#define FSIZE_COMBINE(high, low) (low)
#else
#define FSIZE_LOW(fsize) ((fsize) & 0xffffffff)
#define FSIZE_HIGH(fsize)(((fsize) >> 32) & 0xffffffff)
#define FSIZE_COMBINE(high, low) ((((loff_t) (high)) << 32) | (((loff_t) (low)) & 0xFFFFFFFF))
#endif


#endif
