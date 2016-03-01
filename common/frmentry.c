#include "usrinc.h"

#define OS_START_PRIO   (0)   /* RTOS第一个线程，用于启动一切 */

/** @addtogroup Glory_EmbeddedWorks
 * @{
 */
volatile u32_t sys_tick = 0;    /* 系统时基，每1毫秒+1，切勿手动修改 */
static OS_STK os_start_task_stack[OS_START_STACK_SIZE] @ ".noinit";
static void os_start_task(void *p_arg);

/**
 * function description
 *
 * @param  none
 * @return none
 *
 * @brief  none
 */
int main(void)
{
  // Initialize "uC/OS-II, The Real-Time Kernel"
  OSInit();
  // Create the start task
  OSTaskCreate(os_start_task, NULL,
    &os_start_task_stack[OS_START_STACK_SIZE-1],
    OS_START_PRIO);
  // Start multitasking (i.e. give control to uC/OS-II)
  OSStart();
  // 从来不会执行到这里。
  return 0;
}

/**
 * function description
 *
 * @param  none
 * @return none
 *
 * @brief  none
 */
void os_start_task(void *p_arg)
{
  OSTickInit();
#if (OS_TASK_STAT_EN > 0)
  OSStatInit();
#endif
  DBG_PUTS("\nloading drivers.");
  if(!UserDriversInit()){
    sys_suspend();
  }
  DBG_PUTS("\ndrivers loaded!");
  UserEntryInit();
  UserEntryLoop();      /* 调用用户线程循环 */
  sys_suspend();
}

/**
 * @}
 */

