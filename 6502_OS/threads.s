


  .setcpu "6502"

        .export payload_main
        .import bios_putc

payload_main:
main_loop:
        jsr threadA_step
        jsr delay
        jsr threadB_step
        jsr delay
        jmp main_loop



threadA_step:
        lda #'A'
        jsr bios_putc
        rts

threadB_step:
        lda #'B'
        jsr bios_putc
        rts



delay:
        ldx #$40
d1:     ldy #$FF
d2:     dey
        bne d2
        dex
        bne d1
        rts
