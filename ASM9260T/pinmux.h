#ifndef __HW_PINMUX_H__
#define __HW_PINMUX_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

enum _enum_pinmux {
  PIN_FUNC0 = 0,
  PIN_FUNC1,
  PIN_FUNC2,
  PIN_FUNC3,
  PIN_FUNC4,
  PIN_FUNC5,
  PIN_FUNC6,
  PIN_FUNC7,
};

enum _enum_pinmode {
  PIN_MODE_DISABLE = 0,   // 上拉/下拉电阻禁止。
  PIN_MODE_ENABLE = 2,    // 上拉/下拉电阻使能。
};

bool_t HW_PinMuxInit(void);
void HW_SetPinFunc(u32_t port, u32_t pin, u32_t mux_type /* _enum_pinmux */);
void HW_SetPinMode(u32_t port, u32_t pin, u32_t mode /* _enum_pinmode */);

////////////////////////////////////////////////////////////////////////////////
#endif /* __HW_PINMUX_H__ */

