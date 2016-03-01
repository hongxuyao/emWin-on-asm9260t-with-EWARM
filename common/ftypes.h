#ifndef __FRAME_TYPES__H__
#define __FRAME_TYPES__H__
////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>

typedef signed char         s8_t;
typedef unsigned char       u8_t;
typedef signed short        s16_t;
typedef unsigned short      u16_t;
typedef signed long         s32_t;
typedef unsigned long       u32_t;
typedef signed long long    s64_t;
typedef unsigned long long  u64_t;
typedef float               f32_t;
typedef double              f64_t;
typedef unsigned char       bool_t;
typedef signed int          sbase_t;
typedef unsigned int        ubase_t;

#if defined __LITTLE_ENDIAN__
#if __LITTLE_ENDIAN__ > 0
// 小端字节顺序。
#define IS_BIG_ENDIAN       (0)
#else
// 大端字节顺序。
#define IS_BIG_ENDIAN       (1)
#endif
#else
#error <__LITTLE_ENDIAN__>未定义！
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef TRUE
#define TRUE (!0)
#endif

#ifndef FALSE
#define FALSE (!1)
#endif

////////////////////////////////////////////////////////////////////////////////
#endif /* __FRAME_TYPES__H__ */

