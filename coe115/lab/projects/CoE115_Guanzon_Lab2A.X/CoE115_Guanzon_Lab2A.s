.include "p24FJ64GB002.inc"
.global __reset
config __CONFIG1, FWDTEN_OFF & JTAGEN_OFF
config __CONFIG2, POSCMOD_NONE & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL96MHZ_OFF & PLLDIV_NODIV
config __CONFIG3, SOSCSEL_IO
.bss
i: .space 2
j: .space 2
.text
__reset:
    mov     #__SP_init, W15
    mov     #__SPLIM_init, W0
    mov     W0, SPLIM
    clr     i
    clr     j
    mov #0xFFFF, W0
    mov W0, AD1PCFG
    mov #0xFFC0, W0
    mov W0, TRISB
    mov #0xFFF0, W0
    mov W0, TRISA

main:
    call    lcd_init
    mov     #0x10, W4
    mov     #0x3
    mov     #0x3
    call    lcd_send8bits
    goto    wait

lcd_send4bits:
    mov     W4, LATB        ; Set RS
    call    delay4_1ms      ; t_as / Setup time
    bset    LATB, #0x5      ; Set enable to high
    call    delay4_1ms      ; Data delay time
    mov     LATB, W3
    ior     W1, W3, W1      ; Copy bit5 (Enable) and bit4 (RS) to W1
    mov     W1, LATB        ; Write data to LCD
    call    delay4_1ms      ; PWeh / Enable pulse width and Data setup time
    bclr    LATB, #0x5      ; Set enable to low
    call    delay4_1ms      ; Address/Data hold time and t_cycle
    clr     LATB
    return

lcd_send8bits:
    call    lcd_send4bits   ; Send upper 4 bits
    mov     W2, W1          ; Load lower 4 bits
    call    lcd_send4bits   ; Send lower 4 bits
    return

delay500ms:
    mov #0x000b, W0
    mov W0, i
    mov #0x2C23, W0
    mov W0, j
loop500ms:
    dec j
    bra nz, loop500ms
    dec i
    bra nz, loop500ms
    return

delay100us:
    mov     #0x0090, W0
    mov     W0, j
loop100us:
    dec     j
    bra     nz, loop100us
    return

delay4_1ms:
    mov     #0x1600, W0
    mov     W0, j
loop4_1ms:
    dec     j
    bra     nz, loop4_1ms
    return

delay15ms:
    mov     #0x4F00, W0
    mov     W0, j
loop15ms:
    dec     j
    bra     nz, loop15ms
    return

wait:
    goto wait

.end
