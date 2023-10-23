/**
 ******************************************************************************
 * @addtogroup SMART SMART Initcall infrastructure
 * @{
 * @addtogroup   SMART_INITCALL Generic Initcall Macros
 * @brief Initcall Macros
 * @{
 *
 * @file       smart_initcall.h 
 * @brief      Initcall header
 * @see        * 
 *
 *****************************************************************************/
#ifndef SMART_INITCALL_H
#define SMART_INITCALL_H

/* 
 * This implementation is heavily based on the Linux Kernel initcall
 * infrastructure:
 *   http://lxr.linux.no/#linux/include/linux/init.h
 *   http://git.kernel.org/?p=linux/kernel/git/torvalds/linux-2.6.git;a=blob;f=include/linux/init.h
 */

/*
 * Used for initialization calls..
 */
#include <stdint.h>

typedef int32_t (*initcall_t)(void);
typedef int32_t (*loopercall_t)(void);

typedef struct {
	initcall_t fn_minit;
} initmodule_t;

typedef struct {
	initcall_t fn_minit;
} initdata_t;

typedef struct {
	loopercall_t fn_looper;
} module_looper_t;

/* Init module section */
extern initmodule_t __module_initcall_start[], __module_initcall_end[];

/* Init data init section */
extern initdata_t __data_initcall_start[], __data_initcall_end[];

/* Init task section */
extern module_looper_t __module_looper_start[], __module_looper_end[];

/* initcalls are now grouped by functionality into separate 
 * subsections. Ordering inside the subsections is determined
 * by link order.
 *
 * The `id' arg to __define_initcall() is needed so that multiple initcalls
 * can point at the same handler without causing duplicate-symbol build errors.
 */

/*#define __define_initcall(level,fn,id) \
	static initcall_t __initcall_##fn##id __attribute__((__used__)) \
	__attribute__((__section__(".initcall" level ".init"))) = fn */

#define __define_module_initcall(level, ifn) \
	static initmodule_t __initcall_##fn __attribute__((__used__)) \
	__attribute__((__section__(".initcall" level ".init"))) = { .fn_minit = ifn };

#define MODULE_INITCALL(ifn)		__define_module_initcall("module", ifn)

#define MODULE_INITIALISE_ALL()  { \
		for (initmodule_t *fn = __module_initcall_start; fn < __module_initcall_end; fn++) { \
			if (fn && fn->fn_minit){				\
				(fn->fn_minit)();			\
			} \
		}							\
	}




#define __define_data_initcall(level, ifn) \
	static initdata_t __initcall_##fn __attribute__((__used__)) \
	__attribute__((__section__(".initcall" level ".init"))) = { .fn_minit = ifn };

#define DATA_INITCALL(ifn)		__define_data_initcall("data", ifn)

#define DATA_INITIALISE_ALL()  { \
		for (initdata_t *fn = __data_initcall_start; fn < __data_initcall_end; fn++) { \
			if (fn && fn->fn_minit){				\
				(fn->fn_minit)();			\
			} \
		}							\
	}

#define __define_module_looper(level, ifn) \
	static module_looper_t __loopercall_##fn __attribute__((__used__)) \
	__attribute__((__section__(".looper" level ".looper"))) = { .fn_looper = ifn };

#define MODULE_LOOPER(ifn)		__define_module_looper("module", ifn)

#define MODULE_LOOPER_ALL()  { \
		for (module_looper_t *fn = __module_looper_start; fn < __module_looper_end; fn++) { \
			if (fn && fn->fn_looper){				\
				(fn->fn_looper)();			\
			} \
		}							\
	}


#endif	/* SMART_INITCALL_H */

/**
 * @}
 * @}
 */
