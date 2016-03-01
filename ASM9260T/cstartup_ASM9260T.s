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
    PUBLIC  __image_summary
    PUBLIC  __iar_program_start

    EXTERN  ROM_REGION_SIZE
    EXTERN  MMU_Init
    EXTERN  __iar_data_init3
    EXTERN  UserEarlyConfig
    EXTERN  OS_CPU_IRQ_ISR
    EXTERN  OS_CPU_FIQ_ISR
    EXTERN  main

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

SYS_STK_SIZE  EQU     2048
ABT_STK_SIZE  EQU     2048
UND_STK_SIZE  EQU     2048
IRQ_STK_SIZE  EQU     4096
FIQ_STK_SIZE  EQU     4096
SVC_STK_SIZE  EQU     2048

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
    DC32    __undef_handler
swi_handle:
    DC32    __swi_handler
prefetch_handle:
    DC32    __prefetch_handler
data_handle:
    DC32    __data_handler
reserved_handle:
    DC32    0
irq_handle:
    DC32    OS_CPU_IRQ_ISR
fiq_handle:
    DC32    OS_CPU_FIQ_ISR
__image_summary:                  ; 摘要信息结构必须与sysloader<usrinc.h>定义一致！
    DC32    ROM_REGION_SIZE       ; 4B=计算程序镜像ROM的字节数
    DC8     "ASM920T"             ; 8B="ASM920T\0"
    DC8     0                     ;   字符串结尾符
    DC32    0x00000000            ; 4B=CRC32，有post-build工具计算生成

;********************************************************************************************************
;                                   LOW-LEVEL INITIALIZATION
;********************************************************************************************************
    EXTERN  ARM_UndefinedException
    EXTERN  ARM_DataAbortException
    EXTERN  ARM_PrefAbortException
    EXTERN  ARM_SupervisorException
    EXTERN  ARM_IRQException
    EXTERN  ARM_FIQException

    SECTION .text:CODE:NOROOT(2)
    ARM
    
__dummy_handler:
    MRS     R0, CPSR
    ORR     R0, R0, #ARM_I_BIT|ARM_F_BIT
    MSR     CPSR_c, R0
    B       .
    
__undef_handler:
    ;// 把寄存器保存在异常模式的堆栈中。
    STMFD   SP!, {R0-R3, R12, LR}
    MOV     R0, SP
    MOV     R1, #6
    LDR     R3, =ARM_UndefinedException
    MOV     LR, PC
    BX      R3
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R0-R3, R12, PC}^
    
__swi_handler:
    ;// 把寄存器保存在异常模式的堆栈中。
    STMFD   SP!, {R4, LR}
    LDR     R4, =ARM_SupervisorException
    MOV     LR, PC
    BX      R4
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R4, PC}^
    
__prefetch_handler:
    SUB     LR, LR, #4
    ;// 把寄存器保存在异常模式的堆栈中。
    STMFD   SP!, {R0-R3, R12, LR}
    MOV     R0, SP
    MOV     R1, #6
    LDR     R3, =ARM_PrefAbortException
    MOV     LR, PC
    BX      R3
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R0-R3, R12, PC}^
    
__data_handler:
    SUB     LR, LR, #8
    ;// 把寄存器保存在异常模式的堆栈中。
    STMFD   SP!, {R0-R3, R12, LR}
    MOV     R0, SP
    MOV     R1, #6
    LDR     R3, =ARM_DataAbortException
    MOV     LR, PC
    BX      R3
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R0-R3, R12, PC}^
    
#if 0 /* RTOS有自己的中断管理代码 */
__irq_handler:
    SUB     LR, LR, #4
    ;// 把寄存器保存在IRQ模式的堆栈中。
    STMFD   SP!, {R0-R3, R12, LR}
    LDR     R0, =ARM_IRQException
    MOV     LR, PC
    BX      R0
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R0-R3, R12, PC}^
    
__fiq_handler:
    SUB     LR, LR, #4
    ;// 把寄存器保存在FIQ模式的堆栈中。
    STMFD   SP!, {R0-R3, R12, LR}
    LDR     R0, =ARM_FIQException
    MOV     LR, PC
    BX      R0
    ;// 恢复寄存器并把SPSR恢复到CPSR！
    LDMFD   SP!, {R0-R3, R12, PC}^
#endif

    SECTION .noinit:DATA:NOROOT(3)
__stack_limit:
    DS8     SYS_STK_SIZE
    DS8     ABT_STK_SIZE
    DS8     UND_STK_SIZE
    DS8     IRQ_STK_SIZE
    DS8     FIQ_STK_SIZE
    DS8     SVC_STK_SIZE
__stack_base:
    DS32    1
    
    
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
    LDR     R1, =__stack_base                   ; 堆栈内存的栈底指针
    
    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #UND_MODE                   ; SET UND MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    SUB     R1, R1, #UND_STK_SIZE

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #ABT_MODE                   ; SET ABT MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    SUB     R1, R1, #ABT_STK_SIZE

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #FIQ_MODE                   ; SET FIQ MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    SUB     R1, R1, #FIQ_STK_SIZE

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #IRQ_MODE                   ; SET IRQ MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    SUB     R1, R1, #IRQ_STK_SIZE

    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #SYS_MODE                   ; SET SYSTEM MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    SUB     R1, R1, #SYS_STK_SIZE
    
    BIC     R0, R0, #MODE_BITS                  ; CLEAR THE MODE BITS
    ORR     R0, R0, #SVC_MODE                   ; SET SVC MODE BITS
    MSR     CPSR_c, R0                          ; CHANGE THE MODE
    MOV     SP, R1                              ; 设置堆栈基地址
    
    LDR     R0, =MMU_Init                       ; config and enable MMU
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
    
    LDR     R0, =main
    MOV     LR, PC
    BX      R0
    
__halt_system:
    B       .

    END
