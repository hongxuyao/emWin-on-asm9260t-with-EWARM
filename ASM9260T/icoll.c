#include "usrinc.h"
#include "ASM9260T.h"

////////////////////////////////////////////////////////////////////////////////
static void (* volatile pfIRQTable[64])(void) = { 0 };

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_Init
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t ICOLL_Init(void)
{
  outl((1<<8), REG_SET(HW_AHBCLKCTRL1));
  outl((1<<8), REG_CLR(HW_PRESETCTRL1));
  outl((1<<8), REG_SET(HW_PRESETCTRL1));
  outl((0x0F<<16), REG_CLR(HW_ICOLL_CTRL));
  outl((1<<20), REG_CLR(HW_ICOLL_CTRL));
  for(int i = 0; i < 16; i++){
    u32_t reg = (HW_ICOLL_PRIORITY0 + 16*i);
    outl(~0ul, REG_CLR(reg));
  }
  outl((u32_t)pfIRQTable, HW_ICOLL_VBASE);
  outl(0x00000004, HW_ICOLL_UNDEF_VECTOR);
  outl(~0ul, REG_CLR(HW_ICOLL_CLEAR0));
  outl(~0ul, REG_CLR(HW_ICOLL_CLEAR1));
  // 关闭中断嵌套，使能中断。
  outl((0x0D<<16), REG_SET(HW_ICOLL_CTRL));
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_SetHandler
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_SetHandler(int irqSrc, void (*pf)(void))
{
  u32_t reg = (HW_ICOLL_PRIORITY0+(irqSrc/4)*16);
  int shift = ((irqSrc&3)*8);
  outl((1<<16), REG_CLR(HW_ICOLL_CTRL));
  pfIRQTable[irqSrc] = pf;
  outl((0xF<<shift), REG_CLR(reg));
  outl((1<<16), REG_SET(HW_ICOLL_CTRL));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_EnableIRQ
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_EnableIRQ(int irqSrc)
{
  u32_t reg = (HW_ICOLL_PRIORITY0+(irqSrc/4)*16);
  int shift = ((irqSrc&3)*8);
  outl((0x4<<shift), REG_SET(reg));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_DisableIRQ
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_DisableIRQ(int irqSrc)
{
  u32_t reg = (HW_ICOLL_PRIORITY0+(irqSrc/4)*16);
  int shift = ((irqSrc&3)*8);
  outl((0x4<<shift), REG_CLR(reg));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_StartSoftIRQ
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_StartSoftIRQ(int irqSrc)
{
  u32_t reg = (HW_ICOLL_PRIORITY0+(irqSrc/4)*16);
  int shift = ((irqSrc&3)*8);
  outl((0x8<<shift), REG_SET(reg));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_StopSoftIRQ
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_StopSoftIRQ(int irqSrc)
{
  u32_t reg = (HW_ICOLL_PRIORITY0+(irqSrc/4)*16);
  int shift = ((irqSrc&3)*8);
  outl((0x8<<shift), REG_CLR(reg));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: ICOLL_ProcessIRQ
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void ICOLL_ProcessIRQ(void)
{
  u32_t irqSrc, utmp;
  void (*pf)(void);
  
  irqSrc = (inl(HW_ICOLL_STAT)&0x3F);
  utmp = inl(HW_ICOLL_VECTOR);
  pf = pfIRQTable[irqSrc];
  if(pf){
    pf();
  }
  utmp = (irqSrc&31);
  if(irqSrc < 32){
    outl((1u<<utmp), REG_SET(HW_ICOLL_CLEAR0));
  }else{
    outl((1u<<utmp), REG_SET(HW_ICOLL_CLEAR1));
  }
  outl(0x01, HW_ICOLL_LEVELACK);  /* 应答Level0优先级 */
}


