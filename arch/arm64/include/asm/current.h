/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_CURRENT_H
#define __ASM_CURRENT_H

#include <linux/compiler.h>

#ifndef __ASSEMBLY__

struct task_struct;

/*
 * We don't use read_sysreg() as we want the compiler to cache the value where
 * possible.
 */
static __always_inline struct task_struct *get_current(void)
{
	unsigned long sp_el0;

	asm ("mrs %0, sp_el0" : "=r" (sp_el0)); /* 读取用户空间栈指针寄存器 sp_el0 的值，然后将此值强转成 task_struct 结构就可以获得当前进程。
	                              （sp_el0 里存放的是 init_task，即 thread_info 地址，thread_info 又是在 task_sturct 的开始处，从而找到当前进程。） */
	return (struct task_struct *)sp_el0;
}

#define current get_current()

#endif /* __ASSEMBLY__ */

#endif /* __ASM_CURRENT_H */

