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
    mov #0x0007, W1
    mov #0x000E, W2
    mov #0xFFFF, W3
startup:
    clr LATA
    clr LATB
    call delay
    call delay
    call delay
    call delay
    mov W1, LATA
    mov W2, LATB
    call delay
    call delay
    call delay
    call delay
    mov #0xFFFE, W1
    mov #0xFFFE, W2
    goto forwardA

initA:
    sl.b W1, W1
    bset W1, #0
forwardA:
    mov W1, LATA
    call delay
    mov W3, LATA
    sl.b W1, W1    
    bset W1, #0
    btss W1, #0x3
    goto initB
    goto forwardA

initB:
    mov #0xFFFE, W2
forwardB:
    sl.b W2, W2    
    bset W2, #0
    mov W2, LATB
    call delay
    mov W3, LATB
    btss W2, #0x3
    goto backwardB
    goto forwardB

backwardB:
    lsr W2, W2
    bset W2, #0xF
    mov W2, LATB
    call delay
    mov W3, LATB
    btsc W2, #0x1
    goto backwardB
    goto backwardA

backwardA:
    lsr W1, W1
    bset W1, #0xF
    mov W1, LATA
    call delay
    mov W3, LATA
    btsc W1, #0x0
    goto backwardA
    goto initA

buttonPressed:
    btsc PORTB, #0
    goto buttonReleased
    goto buttonPressed

buttonReleased:
    goto backwardA

delay:
    mov #0x000b, W0
    mov W0, i
    mov #0x2C23, W0
    mov W0, j
loop:
    btss PORTB, #0
    goto buttonPressed
    dec j
    bra nz, loop
    dec i
    bra nz, loop
    return

.end
