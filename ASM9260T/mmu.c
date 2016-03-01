#include "usrinc.h"
#include "ASM9260T.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: MMU_Init
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
void MMU_Init(void)
{
  // 测试清理所有DCache
  MMU_TestCleanDCache();
  // 禁止DCache
  MMU_DisableDCache();
  // 禁止ICache
  MMU_DisableICache();
  // 禁止MMU
  MMU_DisableMMU();
  // 清除DCache
  MMU_InvalidateDCache();
  // 清除ICache
  MMU_InvalidateICache();
  // 清除组相连TLB
  MMU_InvalidateTLB();
  
  // 16KB粗页表清零。
  memset((void*)MMU_TTB, 0, TTB_SIZE);
  memcpy((void*)VECT_ADDR, (void*)EXEC_ADDR, VECT_SIZE);
  
  /* 编写1级页表项,第0项就是地址0的映射,同样需要保证ICache */
  // 物理地址0x200000000-0x20100000(SDRAM)映射到0x0-0x100000
  pTTB[0] = (u32_t)((VECT_ADDR & 0xFFF00000) | MMU_RW_CB);

  // 物理地址0x20100000-0x22000000(SDRAM)映射到0x00100000-0x02000000
  for(int i = 0x001; i < 0x020; i++)
  {
    pTTB[i] = (u32_t)(((0x200 + i) << 20) | MMU_RW_CB);
  }

  // 物理地址0x10000000-0x11000000(NOR)映射到0x10000000-0x11000000
  for(int i = 0x100; i < 0x110; i++)
  {
    pTTB[i] = (u32_t)((i << 20) | MMU_RW_NCNB);
  }

  // 物理地址0x20000000-0x22000000((SDRAM))映射到0x20000000-0x22000000
  for(int i = 0x200; i < 0x220; i++)
  {
    pTTB[i] = (u32_t)((i << 20) | MMU_RW_CB);
  }
  
  // 物理地址0x50000000-0x51000000((AHB外设寄存器))映射到0x50000000-0x51000000
  for(int i = 0x500; i < 0x510; i++)
  {
    pTTB[i] = (u32_t)((i << 20) | MMU_RW_NCNB);
  }

  // 物理地址0x40000000-0x40100000((SRAM))映射到0x40000000-0x40100000
  pTTB[0x400] = (u32_t)((0x400 << 20) | MMU_RW_CB);
  
  // 物理地址0x80000000-0x81000000(寄存器地址)映射到0x80000000-0x81000000
  for(int i = 0x800; i < 0x810; i++)
  {
    pTTB[i] = (u32_t)((i << 20) | MMU_RW_NCNB);
  }
  
  // 物理地址0x90000000-0x90100000(寄存器地址)映射到0x90000000-0x90100000
  pTTB[0x900] = (u32_t)((0x900 << 20) | MMU_RW_NCNB);
  
  MMU_SetTTBase(MMU_TTB);
  MMU_SetDomain(0x55555550|MMU_DOMAIN1_ATTR|MMU_DOMAIN0_ATTR);
  MMU_SetProcessId(0x00<<25);   /* FCSE PID 配置为0x00000000 */
  MMU_EnableAlignFault();
  MMU_EnableMMU();
  MMU_EnableICache();
  MMU_EnableDCache();
}

