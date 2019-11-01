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
    mov #0xFFFC, W0
    mov W0, TRISA
    mov #0xFFF1, W0
    mov W0, TRISB
    bset LATA, #1
main:
waitdown:
    bset LATA, #0
    btsc PORTB, #0
    goto waitdown
waitup:
    bclr LATA, #0
    btss PORTB, #0
    goto waitup
    bset LATA, #0
    bclr LATA, #1
    call delay
    bset LATA, #1
    goto main

delay:
    mov #0x0016, W0
    mov W0, i
    mov #0x2C23, W0
    mov W0, j

loop:
    dec j
    bra nz, loop
    dec i
    bra nz, loop
    return

.end
