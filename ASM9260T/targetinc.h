#ifndef __TARGET_INCLUDE_H__
#define __TARGET_INCLUDE_H__

/** @addtogroup Glory_EmbeddedWorks
 * @{
 */
#include "ftypes.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ASM9260T.h"
#include "icoll.h"
#include "timer0.h"
#include "mmu.h"
#include "ucos_ii.h"

#define DBG_PUTS(s)
#define DBG_PRINT(...)

extern volatile u32_t sys_tick;

static inline u32_t ms2tick(u32_t ms)
{
  u32_t ticks = ms * OS_TICKS_PER_SEC / 1000;
  if((ticks == 0) && (ms != 0)){
    // 若ms为非零，则必须保证返回值非零。
    ticks = 1;
  }
  return ticks;
}

static inline void sys_delay(u32_t ms){
  OSTimeDly(ms2tick(ms));
}

static inline void sys_suspend(void)
{
  OSTaskSuspend(OS_PRIO_SELF);
}

extern bool_t UserDriversInit(void);
extern void UserEntryInit(void);
extern void UserEntryLoop(void);

/**
 * @}
 */

#endif /* __TARGET_INCLUDE_H__ */



