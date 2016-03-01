#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

bool_t HW_GpioInit(void);
void HW_GpioSetDir(u32_t port, u32_t pin, bool_t bOut);
void HW_GpioSetVal(u32_t port, u32_t pin);
void HW_GpioClrVal(u32_t port, u32_t pin);
void HW_GpioToggle(u32_t port, u32_t pin);
int  HW_GpioReadPin(u32_t port, u32_t pin);
u8_t HW_GpioReadPort(u32_t port);
void HW_GpioWritePort(u32_t port, u32_t msk, u32_t val);







////////////////////////////////////////////////////////////////////////////////
#endif /* __HW_GPIO_H__ */

