#ifndef __MMU_H__
#define __MMU_H__
////////////////////////////////////////////////////////////////////////////////
#include "ftypes.h"

#define SDRAM_BASE    (0x20000000)  // 片内SDRAM物理地址
#define SDRAM_SIZE    (32u<<20)     // 片内SDRAM容量
#define VECT_ADDR     (SDRAM_BASE)  // 异常向量表物理地址
#define VECT_SIZE     (1024)        // 异常向量表字节数
#define MMU_TTB       (0x20004000)  // MMU转换表起始地址
#define TTB_SIZE      (16*1024)     // MMU转换表字节数
#define EXEC_ADDR     (0x20008000)  // 应用程序的执行地址
#define LOAD_ADDR     (0x10000)     // 应用程序镜像在QSPI-Flash内的加载地址
#define pTTB          ((volatile u32_t *)MMU_TTB)

////////////////////////////////////////////////////////////////////////////////
// CP15的部分位标志
#define R1_I                (1<<12)
#define R1_C                (1<<2)
#define R1_A                (1<<1)
#define R1_M                (1)
#define R1_iA               (1<<31)
#define R1_nF               (1<<30)

/* it is used for 1MB virtual page */
#define MMU_ATTRIB_SEC      (0x12)  // Section Descriptor
/* C and B bits for Section descriptor */
#define MMU_ATTRIB_CB       (3<<2)  // DCache_on, write_back
#define MMU_ATTRIB_CNB      (2<<2)  // DCache_on, write_through 
#define MMU_ATTRIB_NCB      (1<<2)  // DCache_off, WR_BUF on
#define MMU_ATTRIB_NCNB     (0<<2)  // DCache_off, WR_BUF off
/* Access permission bits (AP) */
#define MMU_ATTRIB_RW       (3<<10) // supervisor=RW, user=RW
#define MMU_ATTRIB_RO       (2<<10) // supervisor=RW, user=RO
#define MMU_ATTRIB_FAULT    (1<<10) // supervisor=RW, user=No Access
/* Config data for Section's domain segment */
#define MMU_ATTRIB_DOM0     (0<<5)  // Domain=0
#define MMU_ATTRIB_DOM1     (1<<5)  // Domain=1
/* Access control bits in Domain access control register */
#define MMU_DOMAIN_FAULT    (0x0)   // Any access generates a domain fault.
#define MMU_DOMAIN_CHK      (0x1)   // Accesses are checked against the access 
                                    // permission bits in the section or 
                                    // page descriptor.
#define MMU_DOMAIN_NOTCHK   (0x3)   // Accesses are not checked against the access 
                                    // permission bits in the section or page 
                                    // descriptor, so a permission fault 
                                    // cannot be generated.
/* General bits setting for bootloader 1MB virtual page */
#define MMU_RW_CB           (MMU_ATTRIB_RW|MMU_ATTRIB_DOM0|MMU_ATTRIB_CB|MMU_ATTRIB_SEC)
#define MMU_RW_CNB          (MMU_ATTRIB_RW|MMU_ATTRIB_DOM0|MMU_ATTRIB_CNB|MMU_ATTRIB_SEC)
#define MMU_RW_NCNB         (MMU_ATTRIB_RW|MMU_ATTRIB_DOM0|MMU_ATTRIB_NCNB|MMU_ATTRIB_SEC)
#define MMU_RW_FAULT        (MMU_ATTRIB_RW|MMU_ATTRIB_DOM1|MMU_ATTRIB_NCNB|MMU_ATTRIB_SEC)
/* Config data for domain0 and domain1 */
#define MMU_DOMAIN0_ATTR    (MMU_DOMAIN_CHK<<0) 
#define MMU_DOMAIN1_ATTR    (MMU_DOMAIN_FAULT<<2) 

void MMU_Init(void);

//====================================
// MMU Cache/TLB/etc on/off functions
//====================================
void MMU_EnableICache(void);
void MMU_DisableICache(void);
void MMU_EnableDCache(void);
void MMU_DisableDCache(void);
void MMU_EnableAlignFault(void);
void MMU_DisableAlignFault(void);
void MMU_EnableMMU(void);
void MMU_DisableMMU(void);
void MMU_SetFastBusMode(void);
void MMU_SetAsyncBusMode(void);
//=========================
// Set TTBase
//=========================
void MMU_SetTTBase(unsigned long base);
void MMU_SetDomain(unsigned long domain);
//=========================
// ICache/DCache functions
//=========================
void MMU_InvalidateIDCache(void);
void MMU_InvalidateICache(void);
void MMU_InvalidateICacheMVA(unsigned long mva);
void MMU_PrefetchICacheMVA(unsigned long mva);
void MMU_InvalidateDCache(void);
void MMU_InvalidateDCacheMVA(unsigned long mva);
void MMU_CleanDCacheMVA(unsigned long mva);
void MMU_CleanInvalidateDCacheMVA(unsigned long mva);
void MMU_CleanDCacheIndex(unsigned long index);
void MMU_CleanInvalidateDCacheIndex(unsigned long index);
void MMU_TestCleanDCache(void);
void MMU_TestCleanInvalidateDCache(void);
void MMU_WaitForInterrupt(void);
//===============
// TLB functions
//===============
void MMU_InvalidateTLB(void);
void MMU_InvalidateTLBMVA(unsigned long mva);
void MMU_InvalidateITLB(void);
void MMU_InvalidateITLBMVA(unsigned long mva);
void MMU_InvalidateDTLB(void);
void MMU_InvalidateDTLBMVA(unsigned long mva);
//=================
// Cache lock down
//=================
void MMU_SetDCacheLockdownBase(unsigned long base);
void MMU_SetICacheLockdownBase(unsigned long base);
//=================
// TLB lock down
//=================
void MMU_SetDTLBLockdown(unsigned long baseVictim);
void MMU_SetITLBLockdown(unsigned long baseVictim);
//============
// Process ID
//============
void MMU_SetProcessId(unsigned long pid);



////////////////////////////////////////////////////////////////////////////////
#endif /* __MMU_H__ */

