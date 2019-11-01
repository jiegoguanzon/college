.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG2, FNOSC_FRC
.bss
i: .space 2
j: .space 2
.text
__reset:
    mov #__SP_init, W15
    mov #__SPLIM_init, W0
    mov W0, SPLIM
    clr i
    clr j
    mov #0xFFFF, W0
    mov W0, AD1PCFG
    mov #0xFFF8, W0
    mov W0, TRISA
    mov #0xFFF1, W0
    mov W0, TRISB
startup:
    clr LATA
    clr LATB
    call delay_2s
    mov #0x0007, W1
    mov #0x000E, W2
    mov W1, LATA
    mov W2, LATB
    call delay_2s

initPatternLoop:
    mov #0xFFFE, W1
    mov #0xFFFE, W2
patternLoop:
    goto patternLoop

delay_500ms:
    mov #0x000B, W0
    mov W0, i
    mov #0x2C23, W0
    mov W0, j
loop_500ms:
    dec j
    bra nz, loop_500ms
    dec i
    bra nz, loop_500ms
    return

delay_2s:
    mov #0x002C, W0
    mov W0, i
    mov #0x2C23, W0
    mov W0, j
loop_2s:
    dec j
    bra nz, loop_2s
    dec i
    bra nz, loop_2s
    return


.end
