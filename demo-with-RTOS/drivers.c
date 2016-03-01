#include "usrinc.h"
#include "icoll.h"

////////////////////////////////////////////////////////////////////////////////
// UserEarlyConfig()函数在启动代码中执行，这时还没有RTOS存在。
////////////////////////////////////////////////////////////////////////////////
bool_t UserEarlyConfig(void)
{
  // 尽早初始化中断控制模块使之可用。
  if(!ICOLL_Init()){
    return FALSE;
  }
  // 将所有USART的时钟源设置为PLL (BUG：UART只能用12MHZ晶振作时钟源)。
  outl(1, REG_SET(HW_UARTCLKSEL));
  outl(1, REG_CLR(HW_UARTCLKUEN));
  outl(1, REG_SET(HW_UARTCLKUEN));
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// UserDriversInit()在最高优先级线程中执行，可以使用RTOS服务。
////////////////////////////////////////////////////////////////////////////////
bool_t UserDriversInit(void)
{
  if(!HW_PinMuxInit()){
    return FALSE;
  }
  if(!HW_GpioInit()){
    return FALSE;
  }
  if(!LED_Init()){
    return FALSE;
  }
  
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
void ARM_UndefinedException(u32_t regs[], int num)
{
  
}

void ARM_DataAbortException(u32_t regs[], int num)
{
  
}

void ARM_PrefAbortException(u32_t regs[], int num)
{
  
}

void ARM_IRQException(void)
{
  ICOLL_ProcessIRQ();
}

void ARM_FIQException(void)
{
  
}

u32_t ARM_SupervisorException(u32_t r0, u32_t r1, u32_t r2, u32_t r3)
{
  return r0;
}

