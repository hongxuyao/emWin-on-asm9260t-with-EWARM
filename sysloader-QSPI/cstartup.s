;
;********************************************************************************************************
;                                    EXCEPTION VECTORS & STARTUP CODE
;
; File      : cstartup.s
; For       : ARM7 or ARM9
; Toolchain : IAR EWARM V5.10 and higher
;********************************************************************************************************
;
    PUBLIC  __vector
    PUBLIC  __vector_0x14
    PUBLIC  __iar_program_start
    PUBLIC  __boot_application

    EXTERN  RAM_REGION_END
    EXTERN  __iar_data_init3
    EXTERN  SysClkConfig
    EXTERN  UserEarlyConfig
    EXTERN  UserDriversInit
    EXTERN  UserEntryInit
    EXTERN  UserEntryLoop
;********************************************************************************************************
;                                           MACROS AND DEFINIITIONS
;********************************************************************************************************
                                ; Mode, correspords to bits 0-5 in CPSR
MODE_BITS   DEFINE    0x1F      ; Bit mask for mode bits in CPSR
USR_MODE    DEFINE    0x10      ; User mode
FIQ_MODE    DEFINE    0x11      ; Fast Interrupt Request mode
IRQ_MODE    DEFINE    0x12      ; Interrupt Request mode
SVC_MODE    DEFINE    0x13      ; Supervisor mode
ABT_MODE    DEFINE    0x17      ; Abort mode
UND_MODE    DEFINE    0x1B      ; Undefined Instruction mode
SYS_MODE    DEFINE    0x1F      ; System mode
ARM_I_BIT   DEFINE    0x80      ; IRQ disable bit
ARM_F_BIT   DEFINE    0x40      ; FIQ disable bit
ARM_T_BIT   DEFINE    0x20      ; Thumb code bit

;********************************************************************************************************
;                                            ARM EXCEPTION VECTORS
;********************************************************************************************************
    SECTION .intvec:CODE:ROOT(2)  ; ROOT标志以确保不被链接程序丢弃
    ARM     ; Always ARM mode after reset

__vector:
    LDR     PC, reset_handle      ; Absolute jump can reach 4 GByte
    LDR     PC, undef_handle      ; Branch to undef_handler
    LDR     PC, swi_handle        ; Branch to swi_handler
    LDR     PC, prefetch_handle   ; Branch to prefetch_handler
    LDR     PC, data_handle       ; Branch to data_handler
__vector_0x14:
    DC32    0x44434241            ; why is 0x44434241 ???
    LDR     PC, irq_handle        ; Branch to irq_handler
    LDR     PC, fiq_handle        ; Branch to fiq_handler

    ; Constant table entries (for ldr pc) will be placed at 0x20
reset_handle:
    DC32    __iar_program_start
undef_handle:
    DC32    __dummy_handler
swi_handle:
    DC32    __dummy_handler
prefetch_handle:
    DC32    __dummy_handler
data_handle:
    DC32    __dummy_handler
reserved_handle:
    DC32    0
irq_handle:
    DC32    __dummy_handler
fiq_handle:
    DC32    __dummy_handler

;********************************************************************************************************
;                                   LOW-LEVEL INITIALIZATION
;********************************************************************************************************

    SECTION .text:CODE:NOROOT(2)
    ARM
    
__dummy_handler:
    MRS     R0, CPSR
    ORR     R0, R0, #ARM_I_BIT|ARM_F_BIT
    MSR     CPSR_c, R0
    B       .
    
    SECTION .text:CODE:NOROOT(2)
    ARM
__boot_application:
    MOV     R1, #SVC_MODE|ARM_I_BIT|ARM_F_BIT
    MSR     CPSR_c, R1
    MOV     LR, PC
    BX      R0  ; 跳转
    B       .
    
    SECTION .text:CODE:NOROOT(2)
    ARM
    
__iar_program_start:
;********************************************************************************************************
;                           ADDITIONAL INITIALIZATION BEFORE SETUP OF STACKPOINTERS
;********************************************************************************************************
    ; 确保当前处于ARM指令，SVC模式，关闭IRQ和FIQ
    MRS     R0, CPSR
    BIC     R0, R0, #MODE_BITS|ARM_T_BIT
    ORR     R0, R0, #SVC_MODE|ARM_I_BIT|ARM_F_BIT
    MSR     CPSR_cxsf, R0
    NOP
    NOP
                                                ; ARM - CP15 c1 Control Register
    MRC     p15, 0, R0, c1, c0, 0               ; Read CP15
    LDR     R1, =0x00001084                     ; 存储小端序，禁止ICache,DCache
    LDR     R2, =0x00004000                     ; 替换策略为：循环替换
    BIC     R0, R0, R1                          ; 清零{R1}对应的位
    ORR     R0, R0, R2                          ; 设置{R2}对应的位
    BIC     R0, R0, #0x00002000                 ; 低地址异常向量表
    BIC     R0, R0, #0x00000001                 ; 禁止MMU
    MCR     p15, 0, R0, c1, c0, 0               ; Write CP15

    MRS     R0, CPSR                            ; ORIGINAL PSR VALUE
    
    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #UND_MODE                   ; SET UND MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF UND_STACK

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #ABT_MODE                   ; SET ABT MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF ABT_STACK

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #FIQ_MODE                   ; SET FIQ MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF FIQ_STACK

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #IRQ_MODE                   ; SET IRQ MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF IRQ_STACK

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #SYS_MODE                   ; SET SYSTEM MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF CSTACK
    
    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #SVC_MODE                   ; SET SVC MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    LDR     SP, =RAM_REGION_END                 ; END OF SVC_STACK
    
    LDR     R0, =SysClkConfig                   ; config CPU clock
    MOV     LR, PC
    BX      R0
    
    LDR     R0, =__iar_data_init3               ; init data segments
    MOV     LR, PC
    BX      R0
    
    LDR     R0, =UserEarlyConfig                ; user's early config
    MOV     LR, PC
    BX      R0
    CMP     R0, #0
    BEQ     __halt_system
    
    LDR     R0, =UserDriversInit                ; user's driver init
    MOV     LR, PC
    BX      R0
    CMP     R0, #0
    BEQ     __halt_system
    
    LDR     R0, =UserEntryInit                  ; user's app init
    MOV     LR, PC
    BX      R0
    
    LDR     R0, =UserEntryLoop                  ; user's app loop
    MOV     LR, PC
    BX      R0
    
__halt_system:
    B       .

    END
