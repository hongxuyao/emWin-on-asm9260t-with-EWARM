;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2010, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : OS_CPU_A.S
; Version   : V2.92
; By        : Jean J. Labrosse
;
* For       : ARM7 & ARM9
* Mode      : ARM and Thumb
; Toolchain : IAR EWARM
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************
    EXTERN  OSRunning                                           ; External references
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSTaskSwHook
    EXTERN  OSIntNesting
    EXTERN  OSIntExit
    
    EXTERN  ARM_IRQException
    EXTERN  ARM_FIQException

    PUBLIC  OSExitCritical
    PUBLIC  OSEnterCritical
    PUBLIC  OSStartHighRdy
    PUBLIC  OSIntCtxSw
    PUBLIC  OSCtxSw
    PUBLIC  OS_CPU_IRQ_ISR
    PUBLIC  OS_CPU_FIQ_ISR

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
ARM_NOINT   EQU       ARM_I_BIT|ARM_F_BIT

;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;********************************************************************************************************
    RSEG    .text:CODE:NOROOT(2)
    ARM
    
OSEnterCritical:
    MRS     R0, CPSR
    ORR     R1, R0, #ARM_NOINT
    MSR     CPSR_c, R1
    BX      LR
    
OSExitCritical:
    MSR     CPSR_c, R0
    BX      LR
    
OSStartHighRdy:
    MSR     CPSR_c, #SVC_MODE|ARM_NOINT

    LDR     R4, =OSRunning
    MOV     R5, #1
    STRB    R5, [R4]
                                            ; SWITCH TO HIGHEST PRIORITY TASK
    LDR     R4, =OSTCBHighRdy               ;    Get highest priority task TCB address
    LDR     R4, [R4]                        ;    get stack pointer
    LDR     SP, [R4]                        ;    switch to the new stack

    LDR     R4, [SP], #4                    ;    pop new task's CPSR
    MSR     SPSR_cxsf, R4
    LDMFD   SP!, {R0-R12,LR,PC}^            ;    pop new task's context

OSIntCtxSw:
    LDR     R4, =OSPrioCur                  ; OSPrioCur = OSPrioHighRdy
    LDR     R5, =OSPrioHighRdy
    LDRB    R6, [R5]
    STRB    R6,[R4]

    LDR     R4, =OSTCBCur                   ; OSTCBCur  = OSTCBHighRdy;
    LDR     R6, =OSTCBHighRdy
    LDR     R6, [R6]
    STR     R6, [R4]

    LDR     SP, [R6]                        ; SP = OSTCBHighRdy->OSTCBStkPtr;

                                            ; RESTORE NEW TASK'S CONTEXT
    LDMFD   SP!, {R4}                       ;    Pop new task's CPSR
    MSR     SPSR_cxsf, R4

    LDMFD   SP!, {R0-R12,LR,PC}^            ;    Pop new task's context
    
OSCtxSw:                                    ; SAVE CURRENT TASK'S CONTEXT
    STMFD   SP!, {LR}                       ;     Push return address
    STMFD   SP!, {LR}
    STMFD   SP!, {R0-R12}                   ;     Push registers
    MRS     R4,  CPSR                       ;     Push current CPSR
    TST     LR,  #1                         ;     See if called from Thumb mode
    ORRNE   R4,  R4, #ARM_T_BIT             ;     If yes, Set the T-bit
    STMFD   SP!, {R4}

    LDR     R4, =OSTCBCur                   ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R5, [R4]
    STR     SP, [R5]

    LDR     R4, =OSPrioCur                  ; OSPrioCur = OSPrioHighRdy
    LDR     R5, =OSPrioHighRdy
    LDRB    R6, [R5]
    STRB    R6, [R4]

    LDR     R4, =OSTCBCur                   ; OSTCBCur  = OSTCBHighRdy;
    LDR     R6, =OSTCBHighRdy
    LDR     R6, [R6]
    STR     R6, [R4]

    LDR     SP, [R6]                        ; SP = OSTCBHighRdy->OSTCBStkPtr;

                                            ; RESTORE NEW TASK'S CONTEXT
    LDMFD   SP!, {R4}                       ;    Pop new task's CPSR
    MSR     SPSR_cxsf, R4

    LDMFD   SP!, {R0-R12,LR,PC}^            ;    Pop new task's context
    
OS_CPU_IRQ_ISR:                             ; Disable FIQ for a moment 
    MSR     CPSR_c, #IRQ_MODE|ARM_NOINT
    STMFD   SP!, {R0-R2}                    ; PUSH WORKING REGISTERS ONTO IRQ STACK
    MOV     R0, SP                          ; Save   IRQ stack pointer
    ADD     SP, SP, #12                     ; Adjust IRQ stack pointer
    SUB     R1, LR, #4                      ; Adjust PC for return address to task
    MRS     R2, SPSR                        ; Copy SPSR (i.e. interrupted task's CPSR) to R2
    MSR     CPSR_c, #SVC_MODE|ARM_NOINT     ; Change to SVC mode
                                            ; SAVE TASK'S CONTEXT ONTO TASK'S STACK
    STMFD   SP!, {R1}                       ;    Push task's Return PC
    STMFD   SP!, {LR}                       ;    Push task's LR
    STMFD   SP!, {R3-R12}                   ;    Push task's R12-R3

    LDMFD   R0!, {R4-R6}                    ;    Move task's R0-R2 from IRQ stack to SVC stack
    STMFD   SP!, {R4-R6}
    STMFD   SP!, {R2}                       ;    Push task's CPSR (i.e. IRQ's SPSR)

                                            ; HANDLE NESTING COUNTER
    LDR     R0, =OSIntNesting               ; OSIntNesting++;
    LDRB    R1, [R0]
    ADD     R1, R1,#1
    STRB    R1, [R0]

    CMP     R1, #1                          ; if (OSIntNesting == 1) {
    BNE     OS_CPU_IRQ_ISR_1

    LDR     R4, =OSTCBCur                   ;     OSTCBCur->OSTCBStkPtr = SP
    LDR     R5, [R4]
    STR     SP, [R5]                        ; }

OS_CPU_IRQ_ISR_1:
    MSR     CPSR_c, #IRQ_MODE|ARM_I_BIT     ; Re-enable FIQ, Change to IRQ mode

    LDR     R0, =ARM_IRQException           ; OS_CPU_IRQ_ISR_Handler();
    MOV     LR, PC
    BX      R0

    MSR     CPSR_c, #SVC_MODE|ARM_NOINT     ; Change to SVC mode

    LDR     R0, =OSIntExit                  ; OSIntExit();
    MOV     LR, PC
    BX      R0
                                            ; RESTORE NEW TASK'S CONTEXT
    LDMFD   SP!, {R4}                       ;    Pop new task's CPSR
    MSR     SPSR_cxsf, R4

    LDMFD   SP!, {R0-R12,LR,PC}^            ;    Pop new task's context
    
OS_CPU_FIQ_ISR:
    STMFD   SP!, {R0-R3}                    ; PUSH WORKING REGISTERS ONTO FIQ STACK
    MOV     R0, SP                          ; Save   FIQ stack pointer
    SUB     R1, LR, #4                      ; Adjust PC for return address to task
    MRS     R2, SPSR                        ; Copy SPSR (i.e. interrupted task's CPSR) to R2 and R3
    MOV     R3, R2

    AND     R3, R3, #MODE_BITS              ; Isolate Mode bits
    CMP     R3, #IRQ_MODE                   ; See if we interrupted an IRQ
    BEQ     OS_CPU_FIQ_ISR_2                ; Branch if yes.

                                            ; =============== FIQ interrupted Task ===============
    MSR     CPSR_c, #SVC_MODE|ARM_NOINT     ; Change to SVC mode
                                            ; SAVE TASK'S CONTEXT ONTO TASK'S STACK
    STMFD   SP!, {R1}                       ;    Push task's Return PC
    STMFD   SP!, {LR}                       ;    Push task's LR
    STMFD   SP!, {R4-R12}                   ;    Push task's R12-R4

    LDMFD   R0!, {R4-R7}                    ;    Move task's R0-R3 from FIQ stack to SVC stack
    STMFD   SP!, {R4-R7}
    STMFD   SP!, {R2}                       ;    Push task's CPSR (i.e. FIQ's SPSR)
        
                                            ; HANDLE NESTING COUNTER
    LDR     R0, =OSIntNesting               ; OSIntNesting++;
    LDRB    R1, [R0]
    ADD     R1, R1,#1
    STRB    R1, [R0]

    CMP     R1, #1                          ; if (OSIntNesting == 1){
    BNE     OS_CPU_FIQ_ISR_1
    LDR     R4, =OSTCBCur                   ;     OSTCBCur->OSTCBStkPtr = SP
    LDR     R5, [R4]
    STR     SP, [R5]                        ; }

OS_CPU_FIQ_ISR_1:
    MSR     CPSR_c, #FIQ_MODE|ARM_NOINT     ; Change to FIQ mode (touse the FIQ stack to handle interrupt)
    ADD     SP, SP, #16                     ; Adjust FIQ stack pointer (Working register were saved)
    LDR     R0, =ARM_FIQException           ; OS_CPU_FIQ_ISR_Handler();
    MOV     LR, PC
    BX      R0

    MSR     CPSR_c, #SVC_MODE|ARM_NOINT     ; Change to SVC mode
    LDR     R0, =OSIntExit                  ; OSIntExit();
    MOV     LR, PC
    BX      R0

                                            ; RESTORE NEW TASK'S CONTEXT
    LDMFD   SP!, {R4}                       ;    Pop new task's CPSR
    MSR     SPSR_cxsf, R4

    LDMFD   SP!, {R0-R12,LR,PC}^            ;    Pop new task's context 

                                            ; =============== FIQ interrupted IRQ ===============
OS_CPU_FIQ_ISR_2:                           ; SAVE IRQ'S CONTEXT ONTO FIQ'S STACK
    STMFD   SP!, {R4-R7,LR}                 ;    Push shared registers
                                            ; HANDLE NESTING COUNTER
    LDR     R0, =OSIntNesting               ; OSIntNesting++;    (Notify uC/OS-II)
    LDRB    R1, [R0]
    ADD     R1, R1,#1
    STRB    R1, [R0]

    LDR     R0, =ARM_FIQException           ; OS_CPU_FIQ_ISR_Handler();
    MOV     LR, PC
    BX      R0
                                            ; HANDLE NESTING COUNTER
    LDR     R0, =OSIntNesting               ; OSIntNesting--;
    LDRB    R1, [R0]                        ;   NO need to call OSIntExit() we are returning to IRQ handler
    SUB     R1, R1, #1
    STRB    R1, [R0]

    LDMFD   SP!, {R4-R7,LR}                 ; Pop IRQ's context
    LDMFD   SP!, {R0-R3}                    ; PULL WORKING REGISTERS ONTO FIQ STACK
    SUBS    PC, LR, #4                      ; Return to IRQ
    
    END
