#include "usrinc.h"
#include "ASM9260T.h"

////////////////////////////////////////////////////////////////////////////////
static void TM0_IRQHandler(void);

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TM0_Init
//| 功能描述 |: 初始化Timer0的通道零为1毫秒定时器。
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t TM0_Init(void)
{
  u32_t pclk;
  
  // 开启Timer0模块的时钟。
  outl((1<<4), REG_SET(HW_AHBCLKCTRL1));
  outl((1<<4), REG_CLR(HW_PRESETCTRL1));
  outl((1<<4), REG_SET(HW_PRESETCTRL1));
  // 关闭Timer0的通道0。
  outl((1<<0), REG_CLR(HW_TIMER0_TCR));
  // Timer0的TC0在每个PCLK上升沿递增。
  outl((3<<0), REG_CLR(HW_TIMER0_CTCR));
  // Timer0的TC0递增计数。
  outl((3<<0), REG_CLR(HW_TIMER0_DIR));
  // Timer0的TC0的与分频为1。
  outl(0, REG_CLR(HW_TIMER0_PR));
  outl(0, REG_CLR(HW_TIMER0_PC));
  // MR0中断使能，匹配时复位TC0。
  outl((7<<0), REG_CLR(HW_TIMER0_MCR));
  outl((3<<0), REG_SET(HW_TIMER0_MCR));
  // 将MR0匹配设置为1毫秒。
  pclk = (inl(HW_SYSPLLCTRL)&0x1FF)*1000000u/4u;
  outl(pclk/1000u, HW_TIMER0_MR0);
  // 使能Timer0的通道0。
  outl((1<<4), REG_SET(HW_TIMER0_TCR));
  outl((1<<4), REG_CLR(HW_TIMER0_TCR));
  outl((1<<0), REG_SET(HW_TIMER0_TCR));
  // 注册中断服务函数。
  ICOLL_SetHandler(INT_TIMER0, TM0_IRQHandler);
  ICOLL_EnableIRQ(INT_TIMER0);
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: TM0_IRQHandler
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void TM0_IRQHandler(void)
{
  u32_t ir = inl(HW_TIMER0_IR);
  if(ir & (1<<0)){
    // 系统时基自增。
    sys_tick++;
    OSTimeTick();
  }
  outl(ir, REG_SET(HW_TIMER0_IR));
}
