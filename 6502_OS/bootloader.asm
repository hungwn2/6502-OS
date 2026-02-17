SRC_LO   = $00
SRC_HI   = $01
DST_LO   = $02
DST_HI   = $03
LEN_LO   = $04
LEN_HI   = $05


;; --------------------------------------------
;; boot
;; --------------------------------------------
boot:
        sei             ; no interrupts
        cld             ; clear decimal mode

        ldx #$ff        ; init CPU stack
        txs

        ;; optional: call BIOS init (LCD, etc.)
        ;; jsr BIOS_INIT

        ;; SRC = PAYLOAD_ROM_START
        lda #<PAYLOAD_ROM_START
        sta SRC_LO
        lda #>PAYLOAD_ROM_START
        sta SRC_HI

        ;; DST = $0200
        lda #<PAYLOAD_RAM_START
        sta DST_LO
        lda #>PAYLOAD_RAM_START
        sta DST_HI

        ;; LEN = PAYLOAD_LEN (16-bit)
        lda #<PAYLOAD_LEN
        sta LEN_LO
        lda #>PAYLOAD_LEN
        sta LEN_HI

        jsr memcpy_rom_to_ram

        ;; jump and execute payload in RAM
        jmp PAYLOAD_RAM_START