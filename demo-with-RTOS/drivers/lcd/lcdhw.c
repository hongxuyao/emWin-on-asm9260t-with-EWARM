#include "usrinc.h"
#include "ASM9260T.h"
#include "lcdhw.h"

////////////////////////////////////////////////////////////////////////////////
// 以下设置水平时序参数，以像素时钟为单位。
#define LCD_HBP         (40-1)
#define LCD_HFP         (22-1)
#define LCD_HSW         (8-1)
#define LCD_PPL         (HW_LCD_X_SIZE-1)
#define DCLK_PL         (LCD_HBP+LCD_HFP+LCD_HSW+LCD_PPL+4)   // 550 DCLK
// 以下设置垂直时序参数，以行时钟为单位。
#define LCD_VBP         (12-1)
#define LCD_VFP         (10-1)
#define LCD_VSW         (6-1)
#define LCD_LPP         (HW_LCD_Y_SIZE-1)
#define LINE_PF         (LCD_VBP+LCD_VFP+LCD_VSW+LCD_LPP+4)   // 300 LINE

#define LCD_FPS         (60)
#define LCD_DCLK        (DCLK_PL*LINE_PF*LCD_FPS)             // 约10MHZ DCLK

typedef enum {
  LCD_1BPP = 0,
  LCD_2BPP,
  LCD_4BPP,
  LCD_8BPP,
  LCD_12_16BPP,
  LCD_24BPP,
} BPPEnumDef;

#if HW_LCD_BPP == 32
#define LCD_BPP         (LCD_24BPP)
#else
#define LCD_BPP         (LCD_12_16BPP)
#endif
#define LCD_PKG_NUM     (16)

////////////////////////////////////////////////////////////////////////////////
typedef struct {
  u32_t NEXT;
  u32_t CHCMD;
  u32_t BUFFER;
  u32_t LCDCTRL0;
} LCD_DMAPKGTypeDef;

#pragma data_alignment=4
static u16_t colorpalette[256] @ ".ncnbram";  /* 调色板，256个条目 */
#if HW_LCD_BPP == 32
#pragma data_alignment=4
static u32_t framebuffer[HW_LCD_X_SIZE*HW_LCD_Y_SIZE] @ ".ncnbram";
#else
#pragma data_alignment=4
static u16_t framebuffer[HW_LCD_X_SIZE*HW_LCD_Y_SIZE] @ ".ncnbram";
#endif
static LCD_DMAPKGTypeDef PixelDMAPkg[LCD_PKG_NUM] @ ".ncnbram";
static LCD_DMAPKGTypeDef LUTblDMAPkg @ ".ncnbram";

////////////////////////////////////////////////////////////////////////////////
static void LCD_InitDMAPackage(void)
{
  LCD_DMAPKGTypeDef volatile *pDMAPkg;
  u32_t pkg_len, lcdctrl, address;
  
  // 初始化用于颜色查找表的DMA包。
  memset(colorpalette, 0, sizeof(colorpalette));
  colorpalette[0] = (LCD_BPP << 12);                  // 用于告诉硬件颜色查找表的类型。
  pDMAPkg = &LUTblDMAPkg;
  pkg_len = ((LCD_BPP == LCD_8BPP) ? 512 : 32);
  pDMAPkg->NEXT     = (u32_t)(&PixelDMAPkg[0]);
  pDMAPkg->CHCMD    = (0x000000c2 | (pkg_len<<16));
  pDMAPkg->BUFFER   = (u32_t)colorpalette;
  pDMAPkg->LCDCTRL0 = (0x30800000 | (pkg_len>>2));
  
  // 初始化用于帧缓存数据的DMA包。
  address = (u32_t)framebuffer;
  pkg_len = ((sizeof(framebuffer))/LCD_PKG_NUM);
  lcdctrl = (0x2c800000 | (pkg_len>>2));              // RGB mode-32bit, so xfer is len/4.
  for(int i = 0; i < LCD_PKG_NUM; i++){
    pDMAPkg = &PixelDMAPkg[i];
    pDMAPkg->NEXT     = (u32_t)(pDMAPkg + 1);
    pDMAPkg->CHCMD    = (0x00001086 | (pkg_len<<16)); // one chain, one word pio write to lcdctrl.
    pDMAPkg->BUFFER   = address;
    pDMAPkg->LCDCTRL0 = lcdctrl;  // PIO Write Value, 被写入HW_LCD_CTRL_CTRL0的值？？？
    address += pkg_len;
  }
  pDMAPkg->NEXT   = (u32_t)(&PixelDMAPkg[0]);
  pDMAPkg->CHCMD |= (1 << 6);     // decrement semaphore.
}

////////////////////////////////////////////////////////////////////////////////
static void LCD_DMAStartAPBH(void *pkg, u32_t chn)
{
  chn = (chn * 0x70);
  outl((u32_t)pkg, (HW_APBH_LCD_CH0_NXTCMDAR+chn));
  outl(1/* always 1 */, (HW_APBH_LCD_CH0_SEMA+chn));
}

////////////////////////////////////////////////////////////////////////////////
static void LCD_ResetTimingAndGo(void)
{
  u32_t dma_xfer;
  
  // LCD DMA init
  outl(~0u, REG_CLR(HW_APBH_LCD_CTRL1));
  outl((1u<<30), REG_CLR(HW_APBH_LCD_CTRL0));
  outl((1u<<31), REG_CLR(HW_APBH_LCD_CTRL0));
  
  // 开启LCD面板时钟。
  outl((1u<<31), REG_CLR(HW_LCD_CTRL_CTRL0));
  outl((1u<<30), REG_SET(HW_LCD_CTRL_CTRL0));
  outl((1u<<30), REG_CLR(HW_LCD_CTRL_CTRL0));
  
  // 设置LCD面板驱动时序。
  outl(((LCD_HBP<<24)|(LCD_HFP<<16)|(LCD_HSW<<10)|LCD_PPL), HW_LCD_CTRL_TIMING0);
  outl(((LCD_VBP<<24)|(LCD_VFP<<16)|(LCD_VSW<<10)|LCD_LPP), HW_LCD_CTRL_TIMING1);
  outl(((3<<24)|(12)), HW_LCD_CTRL_TIMING2);      // DCLK=120M/12=10MHZ，匹配LCD_DCLK宏
  outl((LCD_PPL|(LCD_LPP<<16)), HW_LCD_CTRL_TIMING3);
  outl(((1u<<31)|(1<<29)|(LCD_LPP<<16)), HW_LCD_CTRL_SUBPANEL);
  outl(LCD_LPP, HW_LCD_CTRL_DISPLAYSTATUS);
  outl(128, HW_LCD_LOWTHESHOLD);
  outl(384, HW_LCD_UPTHESHOLD);
  
  // 开启APBH的DMA。
  outl(0x03000700, REG_CLR(HW_APBH_LCD_CTRL0));   // open DMA CLKGATE
  outl(0x02000000, REG_SET(HW_APBH_LCD_CTRL0));   // burst 16

  // 加载颜色查找表。
  LCD_DMAStartAPBH(&LUTblDMAPkg, 0);
  outl(0xfc900081, HW_LCD_CTRL_CTRL1);            // load palette, lcd_en=1 tft
  dma_xfer = ((LCD_BPP == LCD_8BPP) ? 256 : 16);
  outl((0x30000000|dma_xfer), HW_LCD_CTRL_CTRL0); // en_receive and startp and receive n*16bits
  while((inl(HW_LCD_CTRL_STAT)&0x00000040) == 0);
#if HW_LCD_BPP == 32
  // 注意：如果LCD配置为24位色，那么bit23必须为0，否则颜色输出错位。
  outl(0xfc200080, HW_LCD_CTRL_CTRL1);            // load pixel data, lcd_en=0 tft
  outl(0xfc200081, HW_LCD_CTRL_CTRL1);            // load pixel data, lcd_en=1 tft
#else
  outl(0xfca00080, HW_LCD_CTRL_CTRL1);            // load pixel data, lcd_en=0 tft
  outl(0xfca00081, HW_LCD_CTRL_CTRL1);            // load pixel data, lcd_en=1 tft
#endif
  // 开始像素数据的DMA传输。
  outl(0x20000000, REG_SET(HW_APBH_LCD_CTRL0));   // DMA cycle enable
  outl(0x1c000000, REG_CLR(HW_APBH_LCD_CTRL0));
  LCD_DMAStartAPBH(&PixelDMAPkg[0], 0);
  outl(0x04000000, REG_SET(HW_APBH_LCD_CTRL0));   // CH0 cycle enable
  outl(0x03000000, REG_CLR(HW_APBH_LCD_CTRL0));
  outl(0x02000000, REG_SET(HW_APBH_LCD_CTRL0));   // burst 16
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: LCDHW_Init
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |: 
////////////////////////////////////////////////////////////////////////////////
bool_t LCDHW_Init(void)
{
  // 开启LCD模块的时钟并复位。
  outl((1<<14), REG_SET(HW_AHBCLKCTRL1));
  outl((1<<14), REG_CLR(HW_PRESETCTRL1));
  outl((1<<14), REG_SET(HW_PRESETCTRL1));
  outl(4, HW_LCDCLKDIV);  /* 4分频为120MHZ */
  // 初始化连接LCD面板的管脚。
  for(int i = 4; i <= 7; i++){
    HW_SetPinFunc(1, i, PIN_FUNC6);
  }
  for(int i = 0; i <= 7; i++){
    HW_SetPinFunc(2, i, PIN_FUNC6);
  }
  for(int i = 0; i <= 7; i++){
    HW_SetPinFunc(3, i, PIN_FUNC6);
  }
  for(int i = 0; i <= 7; i++){
    HW_SetPinFunc(4, i, PIN_FUNC6);
  }
  // LCD面板背光控制线。
  HW_SetPinFunc(5, 0, PIN_FUNC0);
  HW_SetPinMode(5, 0, PIN_MODE_ENABLE);
  HW_GpioSetDir(5, 0, TRUE);
  HW_GpioSetVal(5, 0);
  
  // 初始化LCD模块并启动工作。
  LCD_InitDMAPackage();
  LCD_ResetTimingAndGo();

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//|          |
//| 函数名称 |: LCDHW_GetVRam
//| 功能描述 |:
//|          |:
//| 参数列表 |:
//|          |:
//| 返    回 |:
//|          |:
//| 备注信息 |:
//|          |: 
////////////////////////////////////////////////////////////////////////////////
u32_t LCDHW_GetVRam(int index)
{
  return (u32_t)framebuffer;
}
