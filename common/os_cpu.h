/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*
*                                (c) Copyright 2006, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                           ARM Cortex-M3 Port
*
* File      : OS_CPU.H
* Version   : V2.92
* By        : Jean J. Labrosse
*
* For       : ARM7 & ARM9
* Mode      : ARM and Thumb
* Toolchain : IAR EWARM
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */

/*
*********************************************************************************************************
*                                              Cortex-M1
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/
#define  OS_CRITICAL_METHOD   3u

#if OS_CRITICAL_METHOD == 3u
#define DECL_CPU_SR          OS_CPU_SR cpu_sr
#define OS_ENTER_CRITICAL()  do{ cpu_sr=OSEnterCritical(); }while(0)
#define OS_EXIT_CRITICAL()   do{ OSExitCritical(cpu_sr); }while(0)
#else
#error  OS_CRITICAL_METHOD 的值不支持！
#endif

/*
*********************************************************************************************************
*                                        Cortex-M3 Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH      (1)                   /* Stack grows from HIGH to LOW memory on ARM        */
#define  OS_TASK_SW()       OSCtxSw()

void OSCtxSw(void);
void OSIntCtxSw(void);
void OSStartHighRdy(void);
OS_CPU_SR OSEnterCritical(void);
void OSExitCritical(OS_CPU_SR sr);
void OS_CPU_IRQ_ISR(void);
void OS_CPU_FIQ_ISR(void);

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

void OSTickInit(void);

#ifdef __cplusplus
}
#endif

#endif
