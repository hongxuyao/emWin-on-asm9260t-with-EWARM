#include "usrinc.h"

////////////////////////////////////////////////////////////////////////////////
#define QUAD_MAX_BUSY_WAIT (0x00040000)
static volatile int quad_spi_cs_flag = 0;

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: QSPI_Init
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
bool_t QSPI_Init(void)
{
  // 使能QSPI时钟。
  outl((1<<1), REG_SET(HW_AHBCLKCTRL1));
  // 复位QSPI模块。
  outl((1<<1), REG_CLR(HW_PRESETCTRL1));
  outl((1<<1), REG_SET(HW_PRESETCTRL1));
  // QSPI模块时钟频率为PLL/4=120MHZ
  outl(4, HW_QUADSPI0CLKDIV);
  // 开启DMA1模块。
  outl((1<<10), REG_SET(HW_AHBCLKCTRL0));
  outl((1<<10), REG_CLR(HW_PRESETCTRL0));
  outl((1<<10), REG_SET(HW_PRESETCTRL0));
  outl(((1<<8)|(0<<0)), HW_DMA1_CHENREG);
  
  // 设置QSPI的6个管脚功能。
  HW_SetPinFunc(11, 0, PIN_FUNC4);
  HW_SetPinFunc(11, 1, PIN_FUNC4);
  HW_SetPinFunc(11, 2, PIN_FUNC4);
  HW_SetPinFunc(11, 3, PIN_FUNC4);
  // 将QSPI_DAT2&QSPI_DAT3配置为普通IO，因为使用两线SPI模式。
  HW_SetPinFunc(11, 4, PIN_FUNC0);
  HW_SetPinFunc(11, 5, PIN_FUNC0);
  HW_SetPinMode(11, 4, PIN_MODE_ENABLE);
  HW_SetPinMode(11, 5, PIN_MODE_ENABLE);
  HW_GpioSetDir(11, 4, FALSE);
  HW_GpioSetDir(11, 5, FALSE);
  
  // 配置QSPI：SPI模式，使用DMA，8位数据帧，10Mbps。
  outl(0x00002038, REG_VAL(HW_QSPI0_CTRL1));
  outl(0x00000205, REG_VAL(HW_QSPI0_TIMING));
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __quad_spi_pinmux_switch
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __quad_spi_pinmux_switch(int state)
{
  if(state){
    HW_SetPinFunc(11, 4, PIN_FUNC4);
    HW_SetPinFunc(11, 5, PIN_FUNC4);
  }else{
    HW_SetPinFunc(11, 4, PIN_FUNC0);
    HW_SetPinFunc(11, 5, PIN_FUNC0);
    HW_SetPinMode(11, 4, PIN_MODE_ENABLE);
    HW_SetPinMode(11, 5, PIN_MODE_ENABLE);
    HW_GpioSetDir(11, 4, FALSE);
    HW_GpioSetDir(11, 5, FALSE);
  }
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __quad_spi_lock_cs
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __quad_spi_lock_cs(void)
{
  quad_spi_cs_flag = 1;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __quad_spi_unlock_cs
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __quad_spi_unlock_cs(void)
{
  quad_spi_cs_flag = 0;
  outl((1<<27), REG_CLR(HW_QSPI0_CTRL0));
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_start_dma_tx
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __qspi_start_dma_tx(void *buf, u32_t bytes)
{
	u32_t temreg;
  
	temreg = ((0<<0)    //INT_EN, ch0 irq disable
    |(0<<1)      // DST_TR_WIDTH, des transfer width, should set to HSIZE, here is 000, means 8bit
      |(0<<4)      // SRC_TR_WIDTH, sor transfer width, should set to HSIZE, here is 000, means 8bit
        |(2<<7)      // DINC, des addr increment, des is SPI, so should set to 1x, means no change
          |(0<<9)      // SINC, sor addr increment, src is sram, so should set to 00, means to increase 
            |(1<<11)     // DEST_MSIZE, des burst length, set to 001 means 4 DST_TR_WIDTH per burst transcation
              |(1<<14)     // SRC_MSIZE, sor burst length, set to 001 means 4 SOR_TR_WIDTH per burst transcation
                |(1<<20)     // TT_FC,transfer type and flow control,001 means memory to peripheral,dma is flow controller
                  |(0<<23)     // DMS, des master select, 0 means ahb master 0
                    |(0<<25)     // SMS, sor master select, 1 means ahb master 1
                      |(0<<27)     // LLP_DST_EN, des block chaining enable, set to 0 disable it
                        |(0<<28));   // LLP_SOR_EN, sor block chaining enable, set to 0 disable it	
  outl((u32_t)buf, HW_DMA1_SAR0);	    // source address
  outl(HW_QSPI0_DATA, HW_DMA1_DAR0);  // destination adress
  
  outl(temreg, HW_DMA1_CTL0);
  outl(bytes, HW_DMA1_CTL0+4);
  
  outl(0, HW_DMA1_CFG0);
  outl(((2<<7)|(2<<11)), HW_DMA1_CFG0+4);
	
  outl(0x01, HW_DMA1_DMACFGREG);
  outl(((1<<8)|(1<<0)), HW_DMA1_CHENREG);
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_start_dma_rx
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __qspi_start_dma_rx(void *buf, u32_t bytes)
{
  u32_t temreg;
  
  temreg = ((0<<0)    //INT_EN, ch0 irq disable
    |(2<<1)      // DST_TR_WIDTH, des transfer width, should set to HSIZE, here is 010, means 32bit
      |(2<<4)      // SRC_TR_WIDTH, sor transfer width, should set to HSIZE, here is 010, means 32bit
        |(0<<7)      // DINC, des addr increment, des is sram, so should set to 00, means to increase
          |(2<<9)      // SINC, sor addr increment, src is SPI, so should set to 1x, means no change 
            |(0<<11)     // DEST_MSIZE, des burst length, set to 000 means 1 DST_TR_WIDTH per burst transcation
					    |(0<<14)     // SRC_MSIZE, sor burst length, set to 000 means 1 SOR_TR_WIDTH per burst transcation
                |(2<<20)     // TT_FC,transfer type and flow control,010 means peripheral to memory,dma is flow controller
                  |(0<<23)     // DMS, des master select, 0 means ahb master 0
                    |(0<<25)     // SMS, sor master select, 1 means ahb master 1
                      |(0<<27)     // LLP_DST_EN, des block chaining enable, set to 0 disable it
                        |(0<<28));    // LLP_SOR_EN, sor block chaining enable, set to 0 disable it
  outl(HW_QSPI0_DATA, HW_DMA1_SAR0);  // source address
  outl((u32_t)buf, HW_DMA1_DAR0);			// destination adress
  
  outl(temreg, HW_DMA1_CTL0);                                
  outl(((bytes+3)/4), HW_DMA1_CTL0+4);
  
  outl(0, HW_DMA1_CFG0);
  outl(((3<<7)|(3<<11)), HW_DMA1_CFG0+4);
  
  outl(0x01, HW_DMA1_DMACFGREG);
  outl(((1<<8)|(1<<0)), HW_DMA1_CHENREG);
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_write_buf
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static int __qspi_write_buf(void *buf, u32_t bytes)
{
  int wait_nops = QUAD_MAX_BUSY_WAIT;
  outl(bytes, HW_QSPI0_XFER);
  outl(((3u<<28)|(quad_spi_cs_flag<<27)), HW_QSPI0_CTRL0);
  __qspi_start_dma_tx(buf, bytes);
  while(((inl(HW_QSPI0_STATUS)&1)||(inl(HW_DMA1_CHENREG)&1))&&(wait_nops-->0));
  if(wait_nops <= 0){
    outl(((1<<8)|(0<<0)), HW_DMA1_CHENREG);
    return -1;
  }
  return bytes;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_read_buf
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static int __qspi_read_buf(void *buf, u32_t bytes)
{
  int wait_nops = QUAD_MAX_BUSY_WAIT;
  outl(bytes, HW_QSPI0_XFER);
  outl(((3u<<28)|(1u<<26)|(quad_spi_cs_flag<<27)), HW_QSPI0_CTRL0);
  __qspi_start_dma_rx(buf, bytes);
  while(((inl(HW_QSPI0_STATUS)&1)||(inl(HW_DMA1_CHENREG)&1))&&(wait_nops-->0));
  if(wait_nops <= 0){
    outl(((1<<8)|(0<<0)), HW_DMA1_CHENREG);
    return -1;
  }
  return bytes;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_w25q_write_enable
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __qspi_w25q_write_enable(int enable)
{
  u8_t qspi_cmd[4];
  qspi_cmd[0] = (enable ? 0x06 : 0x04);
  __quad_spi_lock_cs();
  __qspi_write_buf(qspi_cmd, 1);
  __quad_spi_unlock_cs();
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_w25q_read_status1
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static u8_t __qspi_w25q_read_status1(void)
{
  u8_t qspi_cmd[4];
  qspi_cmd[0] = 0x05;
  __quad_spi_lock_cs();
  __qspi_write_buf(qspi_cmd, 1);
  __qspi_read_buf(qspi_cmd, 4);
  __quad_spi_unlock_cs();
  return qspi_cmd[0];
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_w25q_read_status2
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static u8_t __qspi_w25q_read_status2(void)
{
  u8_t qspi_cmd[4];
  qspi_cmd[0] = 0x35;
  __quad_spi_lock_cs();
  __qspi_write_buf(qspi_cmd, 1);
  __qspi_read_buf(qspi_cmd, 4);
  __quad_spi_unlock_cs();
  return qspi_cmd[0];
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_w25q_quad_enable
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static void __qspi_w25q_quad_enable(int enable)
{
  u8_t qspi_cmd[4];
  
  qspi_cmd[0] = 0x01;
  qspi_cmd[1] = __qspi_w25q_read_status1();
  qspi_cmd[2] = __qspi_w25q_read_status2();
  if(enable){
    qspi_cmd[2] |= 0x02;    // 设置QUAD_ENABLE位
  }else{
    qspi_cmd[2] &= ~0x02;   // 清除QUAD_ENABLE位
  }
  __quad_spi_lock_cs();
  __qspi_write_buf(qspi_cmd, 3);
  __quad_spi_unlock_cs();
  while(__qspi_w25q_read_status1()&1){
    util_fastloop(1000);
  }
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: __qspi_read_flash_quad
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
static int __qspi_read_flash_quad(void *buf, u32_t faddr, u32_t bytes)
{
  u8_t qspi_cmd[8];
  
  // 以SPI模式送出6B命令及flash地址，还附加dumy字节。
  outl(0x07, REG_CLR(HW_QSPI0_CTRL1));
  qspi_cmd[0] = 0x6B;
  qspi_cmd[1] = (faddr >> 16);
  qspi_cmd[2] = (faddr >> 8);
  qspi_cmd[3] = (faddr >> 0);
  qspi_cmd[4] = 0;
  __quad_spi_lock_cs();
  if(5 != __qspi_write_buf(qspi_cmd, 5)){
    __quad_spi_unlock_cs();
    return -1;
  }
  // 以QUAD模式接收W25QXX输出的数据。
  outl((inl(HW_QSPI0_CTRL1)|2), HW_QSPI0_CTRL1);
  if(bytes != __qspi_read_buf(buf, bytes)){
    __quad_spi_unlock_cs();
    return -1;
  }
  __quad_spi_unlock_cs();
  return bytes;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: QSPI_ReadFlash
//| 功能描述 |: 
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |: 
//|          |:
//| 备注信息 |:
//|          |:
////////////////////////////////////////////////////////////////////////////////
int QSPI_ReadFlash(void *buf, u32_t faddr, u32_t bytes)
{
  u32_t n_read, n_left;
  u8_t *ptr;
  
  if(((u32_t)buf%4) || ((u32_t)bytes%4)){
    // 缓存地址和字节数都必须4字节对齐。
    return -1;
  }
  ptr = buf;
  n_left = bytes;
  // 使能W25QXX的QUAD模式，即QUAD_ENABLE。
  __qspi_w25q_write_enable(TRUE);
  __qspi_w25q_quad_enable(TRUE);
  // 将QSPI模块的管脚切换为QUAD模式，only after QUAD_ENABLE!
  __quad_spi_pinmux_switch(TRUE);
  while(n_left){
    n_read = ((n_left > 2048) ? 2048 : n_left);
    if(__qspi_read_flash_quad(ptr, faddr, n_read) != n_read){
      break;
    }
    n_left -= n_read;
    faddr += n_read;
    ptr += n_read;
  }
  outl(0x07, REG_CLR(HW_QSPI0_CTRL1));
  // 将QSPI模块的管脚切换为SPI模式，just before QUAD_DISABLE!
  __quad_spi_pinmux_switch(FALSE);
  // 恢复W25QXX为普通模式，即QUAD_DISABLE。
  __qspi_w25q_write_enable(TRUE);
  __qspi_w25q_quad_enable(FALSE);
  return (int)(bytes-n_left);
}

