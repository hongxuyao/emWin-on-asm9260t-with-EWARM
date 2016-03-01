#ifndef __HW_LCD_H__
#define __HW_LCD_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

////////////////////////////////////////////////////////////////////////////////
// DMA_APBH_LCD
#define HW_LCD_BASE               (0x80800000)
#define HW_APBH_LCD_CTRL0         (HW_LCD_BASE + 0x0000)
#define HW_APBH_LCD_CTRL1         (HW_LCD_BASE + 0x0010)
#define HW_APBH_LCD_CH0_NXTCMDAR  (HW_LCD_BASE + 0x0040)
#define HW_APBH_LCD_CH0_SEMA      (HW_LCD_BASE + 0x0070)
#define HW_APBH_LCD_CH1_NXTCMDAR  (HW_LCD_BASE + 0x00B0)
#define HW_APBH_LCD_CH1_SEMA      (HW_LCD_BASE + 0x00E0)
#define HW_APBH_LCD_CH2_NXTCMDAR  (HW_LCD_BASE + 0x0120)
#define HW_APBH_LCD_CH2_SEMA      (HW_LCD_BASE + 0x0150)

////////////////////////////////////////////////////////////////////////////////
// DMA register
#define HW_LCD_CTRL_BASE          (HW_LCD_BASE + 0x4000) 
#define HW_LCD_CTRL_CTRL0         (HW_LCD_CTRL_BASE + 0x0000)
#define HW_LCD_CTRL_CTRL1         (HW_LCD_CTRL_BASE + 0x0010)
#define HW_LCD_CTRL_Y_RGB_DATA    (HW_LCD_CTRL_BASE + 0x0020)
#define HW_LCD_CTRL_TIMING0       (HW_LCD_CTRL_BASE + 0x0030)
#define HW_LCD_CTRL_TIMING1       (HW_LCD_CTRL_BASE + 0x0040)
#define HW_LCD_CTRL_TIMING2       (HW_LCD_CTRL_BASE + 0x0050)
#define HW_LCD_CTRL_FIFO_STAT     (HW_LCD_CTRL_BASE + 0x0060)
#define HW_LCD_CTRL_STAT          (HW_LCD_CTRL_BASE + 0x0070)
#define HW_LCD_CTRL_SUBPANEL      (HW_LCD_CTRL_BASE + 0x0080)
#define HW_LCD_CTRL_LINEINT       (HW_LCD_CTRL_BASE + 0x0090)
#define HW_LCD_CTRL_DISPLAYSTATUS (HW_LCD_CTRL_BASE + 0x00a0)
#define HW_LCD_CTRL_MONITOR_CTRL  (HW_LCD_CTRL_BASE + 0x00b0)
#define HW_LCD_CTRL_SWITCH_CTRL   (HW_LCD_CTRL_BASE + 0x00c0)
#define HW_LCD_CTRL_CHECK_CTRL    (HW_LCD_CTRL_BASE + 0x00d0)
#define HW_LCD_CTRL_SECURE_CTRL   (HW_LCD_CTRL_BASE + 0x00e0)
#define HW_LCD_CTRL_TIMING3       (HW_LCD_CTRL_BASE + 0x0120)
#define HW_LCD_LOWTHESHOLD        (HW_LCD_CTRL_BASE + 0x0130)
#define HW_LCD_UPTHESHOLD         (HW_LCD_CTRL_BASE + 0x0140)

////////////////////////////////////////////////////////////////////////////////
#define HW_LCD_X_SIZE             (480)
#define HW_LCD_Y_SIZE             (272)
#define HW_LCD_BPP                (16)  /* 取值16或者32 */

////////////////////////////////////////////////////////////////////////////////
bool_t LCDHW_Init(void);
u32_t  LCDHW_GetVRam(int index);

////////////////////////////////////////////////////////////////////////////////
#endif /* __HW_LCD_H__ */

