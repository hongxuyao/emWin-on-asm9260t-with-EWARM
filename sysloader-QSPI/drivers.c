#include "usrinc.h"

////////////////////////////////////////////////////////////////////////////////
void SysClkConfig(void)
{
  u32_t utmp = 0;
  
  // 初始化系统时钟
  outl(1<<2, REG_SET(HW_AHBCLKCTRL0));            // SRAM模块时钟使能
  outl(3<<9, REG_SET(HW_AHBCLKCTRL0));            // DMA0,1模块时钟使能
  outl(1<<8, REG_SET(HW_AHBCLKCTRL1));            // 中断控制模块时钟使能
  outl(inl(HW_PDRUNCFG)&0xFFFFFFFA, HW_PDRUNCFG); // 片外12M晶振和系统PLL上电
  outl(2, HW_CPUCLKDIV);                          // CPUCLK = PLLCLK/2
  outl(2, HW_SYSAHBCLKDIV);                       // AHBCLK = CPUCLK/2
  outl(480, HW_SYSPLLCTRL);                       // PLLCLK = 480MHz 
  while((inl(HW_SYSPLLSTAT)&0x1) == 0x0){         // 等待系统PLL锁定
    if(utmp++ > 0x100000){
      break;
    }
  }
  util_fastloop(40*100);                          // 等待系统时钟稳定100us
  outl(1, HW_MAINCLKSEL);                         // 选择系统PLL为CPU时钟
  outl(0, HW_MAINCLKUEN);                         // 先写0，再写1才能让更新生效
  outl(1, HW_MAINCLKUEN);
  
  // 初始化片内SDRAM
  outl(1<<6, REG_SET(HW_AHBCLKCTRL0));            // 使能EMI模块时钟
  utmp = ((1<<3)                                  // Bank地址位宽=2
         |(12<<5)                                 // 行地址位宽=13
         |(8<<9)                                  // 行地址位宽=9
         |(0<<13));                               // 数据线位宽=16
	outl(utmp, HW_EMI_SCONR);
  utmp = (inl(HW_EMI_CTRL) & 0xFFF1E0FF);
  utmp |= ((5<<8)                                 // EMI内部hclk_d延时调整
          |(1<<17)                                // 片内SDRAM使能
          |(2<<18));                              // 片内SDRAM对应EMI片选2
	outl(utmp, HW_EMI_CTRL);
	outl(0x20000000, HW_EMI_SCSLR2_LOW);            // EMI片选2的物理地址是0x20000000
	outl(0xa, HW_EMI_SMSKR2);                       // EMI片选2的内存大小为32MB
  utmp = ((0<<27)                                 // extended_t_xsr
         |(0<<26)                                 // extended_cas_latency
         |(9<<22)                                 // t_rc=10 clks
         |(2<<18)                                 // t_xsr=3 clks
         |(6<<14)                                 // t_rcar=7 clks
         |(1<<12)                                 // t_wr=2 clks
         |(3<<9)                                  // t_rp=4 clks
         |(3<<6)                                  // t_rcd=4 clks
         |(6<<2)                                  // t_ras_min=7 clks
         |(1<<0));                                // cas_latency=2 clks
  // 0x024996d9 = 00000,0,1001,0010,0110,01,011,011,0110,01
	outl(utmp, HW_EMI_STMG0R);
	outl(0x00542b4f, HW_EMI_SMTMGR_SET0);
	outl(inl(HW_EMI_SCTLR)|(1<<9), HW_EMI_SCTLR);	
}

////////////////////////////////////////////////////////////////////////////////
bool_t UserEarlyConfig(void)
{
  if(!HW_PinMuxInit()){
    return FALSE;
  }
  if(!HW_GpioInit()){
    return FALSE;
  }
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
bool_t UserDriversInit(void)
{
  if(!CLKO_Init()){
    return FALSE;
  }
  if(!LED_Init()){
    return FALSE;
  }
  if(!QSPI_Init()){
    return FALSE;
  }
  
  return TRUE;
}



