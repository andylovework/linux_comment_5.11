// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2002-3 Patrick Mochel
 * Copyright (c) 2002-3 Open Source Development Labs
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/memory.h>
#include <linux/of.h>

#include "base.h"

/**
 * driver_init - initialize driver model.
 *
 * Call the driver model init functions to initialize their
 * subsystems. Called early from init/main.c.
 */
void __init driver_init(void)
{
	/* These are the core pieces */
	devtmpfs_init(); /* 注册一个名为devtmpfs的文件系统dev_fs_type */
	devices_init(); /* 创建devices相关的设备模型 */
	buses_init(); /* 初始化设备模型总线部分的顶级节点 */
	classes_init(); /* 初始化设备模型类部分的顶级容器节点 */
	firmware_init(); /* 初始化设备模型中firmware部分的顶级节点 */
	hypervisor_init(); /* 初始化hypervisor_kobj的顶级容器节点 */

	/* These are also core pieces, but must come after the
	 * core core pieces.
	 */
	of_core_init(); /* 进行设备树相关的核心注册 */
	platform_bus_init(); /* 初始化platform总线 */
	auxiliary_bus_init();
	cpu_dev_init(); /* 初始化cpu设备 */
	memory_dev_init(); /* 初始化内存设备 */
	container_dev_init(); /* 调用subsys_system_register()将container注册到sysfs中 */
}
