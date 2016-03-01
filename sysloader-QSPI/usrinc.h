#ifndef __USER_INCLUDES_H__
#define __USER_INCLUDES_H__

/** @addtogroup Glory_EmbeddedWorks
 * @{
 */
#include "ftypes.h"
#include <intrinsics.h>
#include "ASM9260T.h"
#include "pinmux.h"
#include "mmu.h"
#include "gpio.h"
#include "led/led.h"
#include "qspi/qspi.h"
#include "clkout/clkout.h"

/* 该sysloader所引导的镜像摘要 */
typedef volatile struct {
  u32_t size;
  char  magic[8];
  u32_t crc32;
} tImageSummary;

u32_t util_rev(u32_t n);
u32_t util_rev32(u32_t n);
void  util_fastloop(ubase_t n);
void  __boot_application(u32_t entry);

/**
 * @}
 */

#endif /* __USER_INCLUDES_H__ */



