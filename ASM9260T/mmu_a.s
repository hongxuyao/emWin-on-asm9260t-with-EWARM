            MODULE  MMU_ASM
            
            
;// CP15的部分位标志
R1_iA       EQU         (1<<31)
R1_nF       EQU         (1<<30)
R1_I        EQU         (1<<12)
R1_C        EQU         (1<<2)
R1_A        EQU         (1<<1)
R1_M        EQU         (1<<0)

;///////////////////////////////////////////////////////////////////////////////
;//====================================
;// MMU Cache/TLB/etc on/off functions
;//====================================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableICache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_EnableICache
MMU_EnableICache:
            MRC         P15,0,R0,C1,C0,0
            ORR         R0,R0,#R1_I
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableICache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_DisableICache
MMU_DisableICache:
            MRC         P15,0,R0,C1,C0,0
            BIC         R0,R0,#R1_I
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_EnableDCache
MMU_EnableDCache:
            MRC         P15,0,R0,C1,C0,0
            ORR         R0,R0,#R1_C
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_DisableDCache
MMU_DisableDCache:
            MRC         P15,0,R0,C1,C0,0
            BIC         R0,R0,#R1_C
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableAlignFault(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_EnableAlignFault 
MMU_EnableAlignFault:
            MRC         P15,0,R0,C1,C0,0
            ORR         R0,R0,#R1_A
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableAlignFault(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_DisableAlignFault
MMU_DisableAlignFault:
            MRC         P15,0,R0,C1,C0,0
            BIC         R0,R0,#R1_A
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_EnableMMU(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_EnableMMU
MMU_EnableMMU:
            MRC         P15,0,R0,C1,C0,0
            ORR         R0,R0,#R1_M
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_DisableMMU(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_DisableMMU
MMU_DisableMMU:
            MRC         P15,0,R0,C1,C0,0
            BIC         R0,R0,#R1_M
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetFastBusMode(void);
;// 仅当 FCLK:HCLK = 1:1 时，设置为该模式。
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetFastBusMode
MMU_SetFastBusMode:
            MRC         P15,0,R0,C1,C0,0
            BIC         R0,R0,#R1_iA|R1_nF
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetAsyncBusMode(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetAsyncBusMode
MMU_SetAsyncBusMode:
            MRC         P15,0,R0,C1,C0,0
            ORR         R0,R0,#R1_nF|R1_iA
            MCR         P15,0,R0,C1,C0,0
            BX          LR
        
;//=========================
;// Set TTBase
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetTTBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetTTBase
MMU_SetTTBase:
            MCR         P15,0,R0,C2,C0,0
            BX          LR
        
;//=========================
;// Set Domain
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDomain(unsigned long domain);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetDomain
MMU_SetDomain:
            MCR         P15,0,R0,C3,C0,0
            BX          LR
        
;//=========================
;// ICache/DCache functions
;//=========================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateIDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateIDCache
MMU_InvalidateIDCache:
            MCR         P15,0,R0,C7,C7,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateICache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateICache
MMU_InvalidateICache:
            MCR         P15,0,R0,C7,C5,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateICacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateICacheMVA
MMU_InvalidateICacheMVA:
            MCR         P15,0,R0,C7,C5,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_PrefetchICacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_PrefetchICacheMVA
MMU_PrefetchICacheMVA:
            MCR         P15,0,R0,C7,C13,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateDCache
MMU_InvalidateDCache:
            MCR         P15,0,R0,C7,C6,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateDCacheMVA
MMU_InvalidateDCacheMVA:
            MCR         P15,0,R0,C7,C6,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_CleanDCacheMVA
MMU_CleanDCacheMVA:
            MCR         P15,0,R0,C7,C10,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanInvalidateDCacheMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_CleanInvalidateDCacheMVA
MMU_CleanInvalidateDCacheMVA:
            MCR         P15,0,R0,C7,C14,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanDCacheIndex(unsigned long index);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_CleanDCacheIndex
MMU_CleanDCacheIndex:
            MCR         P15,0,R0,C7,C10,2
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_CleanInvalidateDCacheIndex(unsigned long index);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_CleanInvalidateDCacheIndex
MMU_CleanInvalidateDCacheIndex:
            MCR         P15,0,R0,C7,C14,2
            BX          LR
            
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_TestCleanDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_TestCleanDCache
MMU_TestCleanDCache:
            MRC         P15,0,R15,C7,C10,3
            BNE         MMU_TestCleanDCache
            BX          LR
            
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_TestCleanInvalidateDCache(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_TestCleanInvalidateDCache
MMU_TestCleanInvalidateDCache:
            MRC         P15,0,R15,C7,C14,3
            BNE         MMU_TestCleanInvalidateDCache
            BX          LR
            
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_WaitForInterrupt(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_WaitForInterrupt 
MMU_WaitForInterrupt:
            MCR         P15,0,R0,C7,C0,4
            BX          LR
        
;//===============
;// TLB functions
;//===============
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateTLB(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateTLB
MMU_InvalidateTLB:
            MCR         P15,0,R0,C8,C7,0
            BX          LR
            
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateTLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateTLBMVA
MMU_InvalidateTLBMVA:
            MCR         P15,0,R0,C8,C7,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateITLB(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateITLB
MMU_InvalidateITLB:
            MCR         P15,0,R0,C8,C5,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateITLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateITLBMVA
MMU_InvalidateITLBMVA:
            MCR         P15,0,R0,C8,C5,1
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDTLB(void);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateDTLB
MMU_InvalidateDTLB:
            MCR         P15,0,R0,C8,C6,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_InvalidateDTLBMVA(unsigned long mva);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_InvalidateDTLBMVA 
MMU_InvalidateDTLBMVA:
            MCR         P15,0,R0,C8,C6,1
            BX          LR
        
;//=================
;// Cache lock down
;//=================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDCacheLockdownBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetDCacheLockdownBase 
MMU_SetDCacheLockdownBase:
            ;// R0= victim & lockdown base
            MCR         P15,0,R0,C9,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetICacheLockdownBase(unsigned long base);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetICacheLockdownBase
MMU_SetICacheLockdownBase:
            ;// R0= victim & lockdown base
            MCR         P15,0,R0,C9,C0,1
            BX          LR
        
;//=================
;// TLB lock down
;//=================
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetDTLBLockdown(unsigned long baseVictim);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetDTLBLockdown
MMU_SetDTLBLockdown:
            MCR         P15,0,R0,C10,C0,0
            BX          LR
        
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetITLBLockdown(unsigned long baseVictim);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetITLBLockdown
MMU_SetITLBLockdown:
            MCR         P15,0,R0,C10,C0,1
            BX          LR
        
;//============
;// Process ID
;//============
;///////////////////////////////////////////////////////////////////////////////
;// void MMU_SetProcessId(unsigned long pid);
;///////////////////////////////////////////////////////////////////////////////
            RSEG        .text:CODE:NOROOT(2)
            ARM
            PUBLIC      MMU_SetProcessId
MMU_SetProcessId:
            MCR         P15,0,R0,C13,C0,0
            BX          LR
            
;///////////////////////////////////////////////////////////////////////////////
            END



