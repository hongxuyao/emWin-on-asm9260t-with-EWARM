            MODULE  TOOLBOX_ASM
            
;///////////////////////////////////////////////////////////////////////////////
            RSEG    .text:CODE:NOROOT(2)
            THUMB
            PUBLIC  util_fastloop
            ; void util_fastloop(ubase_t n);
util_fastloop:
            SUBS    R0, R0, #1
            BNE     util_fastloop
            BX      LR

            RSEG    .text:CODE:NOROOT(2)
            ARM
            PUBLIC  util_rev32
            ; u32_t util_rev32(u32_t n);
util_rev32:
            MVN     R2, #0x0000FF00
            EOR     R1, R0, R0, ROR #16
            AND     R1, R2, R1, LSR #8
            EOR     R0, R1, R0, ROR #8
            BX      LR

            RSEG    .text:CODE:NOROOT(2)
            ARM
            PUBLIC  util_rev
            ; u32_t util_rev(u32_t n);
util_rev:
            EOR     R1, R0, R0, ROR #16
            MOV     R1, R1, LSR #8
            BIC     R1, R1, #0xFF00
            EOR     R0, R1, R0, ROR #8
            BX      LR

;///////////////////////////////////////////////////////////////////////////////
            END

