#ifndef __USER_INCLUDES_H__
#define __USER_INCLUDES_H__

/** @addtogroup Glory_EmbeddedWorks
 * @{
 */
#include "ftypes.h"
#include <intrinsics.h>
#include "targetinc.h"
#include "pinmux.h"
#include "gpio.h"
#include "led/led.h"
#include "lcd/lcdhw.h"

// 在此声明各个线程的堆栈数组及其大小（不是定义，是声明！）
#define TASK_GUI_PRIO       (OS_TASK_STAT_PRIO-1) /* GUI优先级最低 */
#define TASK_GUI_STK        (8192)
extern OS_STK task_gui_stk[];
extern void task_gui(void *p_arg);

/**
 * @}
 */

#endif /* __USER_INCLUDES_H__ */



