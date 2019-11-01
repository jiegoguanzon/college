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
    mov #0xFFFF, W0
    mov W0, TRISA
    call    lcd_init

main:
    call    showView1
    call    animateView1
    call    clearDisplay
    call    showView2
    call    animateView2
    call    waitForButtonPress
    call    clearDisplay
    call    showView3
    call    animateView3
    call    delay10s
    call    clearDisplay
    goto    main

showView1:
    call    writeName
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x9, W1
    mov     #0x8, W2
    call    lcd_send8bits
    call    writeName
    call    startCursorAt2ndLine
    call    writeStudentNum
    mov     #0x00, W4       ; Point RS to IR
    mov     #0xD, W1
    mov     #0x8, W2
    call    lcd_send8bits
    call    writeStudentNum
    mov     #0x0, W5
    return

showView2:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x3, W1
    mov     #0x1, W2
    call    lcd_send8bits   ; Print 1
    mov     #0x3, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print 7
    mov     #0x3, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Print 0
    mov     #0xA, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Print space
    mov     #0x6, W1
    mov     #0x3, W2
    call    lcd_send8bits   ; Print c
    mov     #0x6, W1
    mov     #0xD, W2
    call    lcd_send8bits   ; Print m
    mov     #0x00, W4       ; Point RS to IR
    mov     #0xC, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Move cursor to start of 2nd line
    mov     #0x10, W4       ; Point RS to DR
    mov     #0xA, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print space
    mov     #0x3, W1
    mov     #0x5, W2        
    call    lcd_send8bits   ; Print 5
    mov     #0x3, W1
    mov     #0x5, W2        
    call    lcd_send8bits   ; Print 5
    mov     #0xA, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print space
    mov     #0x6, W1
    mov     #0xB, W2        
    call    lcd_send8bits   ; Print k
    mov     #0x6, W1
    mov     #0x7, W2        
    call    lcd_send8bits   ; Print g
    mov     #0x0, W5        ; Initialize counter
    mov     #0x1, W6        ; Set direction to increment
    return

showView3:
    call    showView3_0
    mov     #0x0, W5
    mov     #0x1, W6
    return

showView3_0:
    call    writeMcdo
    call    startCursorAt2ndLine
    call    writeJollibee
    return

showView3_1:
    call    writeJollibee
    call    startCursorAt2ndLine
    call    writeChowKing
    return

showView3_2:
    call    writeChowKing
    call    startCursorAt2ndLine
    call    writeKFC
    return

showView3_3:
    call    writeKFC
    call    startCursorAt2ndLine
    call    writeBurgerKing
    return

animateView1:
    btss    PORTA, #0x0     ; Skip if pushbutton is not pressed
    return
    call    shiftDisplayRight
    inc     W5, W5
    cp      W5, #0x10
    btsc    SR, #0x1
    call    displayReturnHome
    btsc    SR, #0x1
    mov     #0x0, W5
    call    delay500ms
    goto    animateView1

animateView2:
    btsc    PORTA, #0x0     ; Skip if pushbutton is pressed
    return
    btsc    W6, #0x0        ; If increasing, shift right
    call    shiftDisplayRight
    btss    W6, #0x0        ; If decreasing, shift left
    call    shiftDisplayLeft
    btsc    W6, #0x0        ; If increasing, increment
    inc     W5, W5
    btss    W6, #0x0        ; If decreasing, decrement
    dec     W5, W5
    cp      W5, #0xA        ; Check if counter == 10
    btsc    SR, #0x1
    btg     W6, #0x0        ; Toggle direction
    cp      W5, #0x0        ; Check if counter == 0
    btsc    SR, #0x1
    btg     W6, #0x0        ; Toggle direction
    call    delay500ms
    goto    animateView2

animateView3:
    btsc    PORTA, #0x0     ; Skip if pushbutton is pressed
    return
    call    delay500ms
    call    delay500ms
    call    clearDisplay
    btsc    W6, #0x0        ; If increasing, increment
    inc     W5, W5
    btss    W6, #0x0        ; If decreasing, decrement
    dec     W5, W5
    cp      W5, #0x0        ; If counter == 0
    btsc    SR, #0x1
    call    showView3_0
    btsc    SR, #0x1
    btg     W6, #0x0
    cp      W5, #0x1        ; If counter == 1
    btsc    SR, #0x1
    call    showView3_1
    cp      W5, #0x2        ; If counter == 2
    btsc    SR, #0x1
    call    showView3_2
    cp      W5, #0x3        ; If counter == 3
    btsc    SR, #0x1
    call    showView3_3
    btsc    SR, #0x1
    btg     W6, #0x0
    goto    animateView3

writeName:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0xA, W2        
    call    lcd_send8bits   ; Print J
    mov     #0xA, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print space
    mov     #0x4, W1
    mov     #0x7, W2        
    call    lcd_send8bits   ; Print G
    mov     #0x7, W1
    mov     #0x5, W2        
    call    lcd_send8bits   ; Print u
    mov     #0x6, W1
    mov     #0x1, W2        
    call    lcd_send8bits   ; Print a
    mov     #0x6, W1
    mov     #0xE, W2        
    call    lcd_send8bits   ; Print n
    mov     #0x7, W1
    mov     #0xA, W2        
    call    lcd_send8bits   ; Print z
    mov     #0x6, W1
    mov     #0xF, W2        
    call    lcd_send8bits   ; Print o
    mov     #0x6, W1
    mov     #0xE, W2        
    call    lcd_send8bits   ; Print n
    return

writeStudentNum:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x3, W1
    mov     #0x2, W2        
    call    lcd_send8bits   ; Print 2
    mov     #0x3, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print 0
    mov     #0x3, W1
    mov     #0x1, W2        
    call    lcd_send8bits   ; Print 1
    mov     #0x3, W1
    mov     #0x5, W2        
    call    lcd_send8bits   ; Print 5
    mov     #0x3, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print 0
    mov     #0x3, W1
    mov     #0x7, W2        
    call    lcd_send8bits   ; Print 7
    mov     #0x3, W1
    mov     #0x9, W2        
    call    lcd_send8bits   ; Print 9
    mov     #0x3, W1
    mov     #0x0, W2        
    call    lcd_send8bits   ; Print 0
    mov     #0x3, W1
    mov     #0x6, W2        
    call    lcd_send8bits   ; Print 6
    return

writeMcdo:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0xD, W2
    call    lcd_send8bits   ; Print M
    mov     #0x6, W1
    mov     #0x3, W2
    call    lcd_send8bits   ; Print c
    mov     #0x4, W1
    mov     #0x4, W2
    call    lcd_send8bits   ; Print D
    mov     #0x6, W1
    mov     #0xF, W2
    call    lcd_send8bits   ; Print o
    mov     #0x6, W1
    mov     #0xE, W2
    call    lcd_send8bits   ; Print n
    mov     #0x6, W1
    mov     #0x1, W2
    call    lcd_send8bits   ; Print a
    mov     #0x6, W1
    mov     #0xC, W2
    call    lcd_send8bits   ; Print l
    mov     #0x6, W1
    mov     #0x4, W2
    call    lcd_send8bits   ; Print d
    mov     #0x2, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print apostrophe
    mov     #0x7, W1
    mov     #0x3, W2
    call    lcd_send8bits   ; Print s
    return

writeKFC:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0xB, W2
    call    lcd_send8bits   ; Print K
    mov     #0x4, W1
    mov     #0x6, W2
    call    lcd_send8bits   ; Print F
    mov     #0x4, W1
    mov     #0x3, W2
    call    lcd_send8bits   ; Print C
    return

writeChowKing:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0x3, W2
    call    lcd_send8bits   ; Print C
    mov     #0x6, W1
    mov     #0x8, W2
    call    lcd_send8bits   ; Print h
    mov     #0x6, W1
    mov     #0xF, W2
    call    lcd_send8bits   ; Print o
    mov     #0x7, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print w
    mov     #0xA, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Print space
    mov     #0x4, W1
    mov     #0xB, W2
    call    lcd_send8bits   ; Print K
    mov     #0x6, W1
    mov     #0x9, W2
    call    lcd_send8bits   ; Print i
    mov     #0x6, W1
    mov     #0xE, W2
    call    lcd_send8bits   ; Print n
    mov     #0x6, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print g
    return

writeBurgerKing:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0x2, W2
    call    lcd_send8bits   ; Print B
    mov     #0x7, W1
    mov     #0x5, W2
    call    lcd_send8bits   ; Print u
    mov     #0x7, W1
    mov     #0x2, W2
    call    lcd_send8bits   ; Print r
    mov     #0x6, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print g
    mov     #0x6, W1
    mov     #0x5, W2
    call    lcd_send8bits   ; Print e
    mov     #0x7, W1
    mov     #0x2, W2
    call    lcd_send8bits   ; Print r
    mov     #0xA, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Print space
    mov     #0x4, W1
    mov     #0xB, W2
    call    lcd_send8bits   ; Print K
    mov     #0x6, W1
    mov     #0x9, W2
    call    lcd_send8bits   ; Print i
    mov     #0x6, W1
    mov     #0xE, W2
    call    lcd_send8bits   ; Print n
    mov     #0x6, W1
    mov     #0x7, W2
    call    lcd_send8bits   ; Print g
    return

writeJollibee:
    mov     #0x10, W4       ; Point RS to DR
    mov     #0x4, W1
    mov     #0xA, W2
    call    lcd_send8bits   ; Print J
    mov     #0x6, W1
    mov     #0xF, W2
    call    lcd_send8bits   ; Print o
    mov     #0x6, W1
    mov     #0xC, W2
    call    lcd_send8bits   ; Print l
    mov     #0x6, W1
    mov     #0xC, W2
    call    lcd_send8bits   ; Print l
    mov     #0x6, W1
    mov     #0x9, W2
    call    lcd_send8bits   ; Print i
    mov     #0x6, W1
    mov     #0x2, W2
    call    lcd_send8bits   ; Print b
    mov     #0x6, W1
    mov     #0x5, W2
    call    lcd_send8bits   ; Print e
    mov     #0x6, W1
    mov     #0x5, W2
    call    lcd_send8bits   ; Print e
    return

lcd_init:
    call    delay15ms
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x3, W1        ; Function Set Command: (8-bit interface)
    call    lcd_send4bits   ; Send command
    call    delay4_1ms      
    mov     #0x3, W1        ; Function Set Command: (8-bit interface)
    call    lcd_send4bits   ; Send same command
    call    delay100us
    mov     #0x3, W1        ; Function Set Command: (8-bit interface)
    call    lcd_send4bits   ; Send same command again
    mov     #0x2, W1        ; Set interface to 4-bit
    call    lcd_send4bits   ; Send command
    mov     #0x2, W1        ; Set interface to 4-bit
    mov     #0x8, W2        ; Set N and F to zero b'NFxx'
    call    lcd_send8bits   ; Send 8-bit command
    mov     #0x0, W1
    mov     #0x8, W2
    call    lcd_send8bits   ; Send Display OFF command
    call    clearDisplay
    mov     #0x0, W1
    mov     #0x6, W2        ; Entry Mode Set
    call    lcd_send8bits   ; Send Entry Mode command
    mov     #0x0, W1
    mov     #0xC, W2        
    call    lcd_send8bits   ; Send Display ON command
    return

clearDisplay:
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x0, W1
    mov     #0x1, W2
    call    lcd_send8bits   ; Send Clear Display command
    return

displayReturnHome:
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x0, W1
    mov     #0x2, W2
    call    lcd_send8bits
    return

shiftDisplayRight:
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x1, W1
    mov     #0xC, W2
    call    lcd_send8bits
    return

shiftDisplayLeft:
    mov     #0x00, W4       ; Point RS to IR
    mov     #0x1, W1
    mov     #0x8, W2
    call    lcd_send8bits
    return

startCursorAt2ndLine:
    mov     #0x00, W4       ; Point RS to IR
    mov     #0xC, W1
    mov     #0x0, W2
    call    lcd_send8bits   ; Move cursor to start of 2nd line
    return

lcd_send4bits:
    mov     W4, LATB        ; Set RS
    call    delay500us      ; t_as / Setup time
    bset    LATB, #0x5      ; Set enable to high
    call    delay500us      ; Data delay time
    mov     LATB, W3
    ior     W1, W3, W1      ; Copy bit5 (Enable) and bit4 (RS) to W1
    mov     W1, LATB        ; Write data to LCD
    call    delay500us      ; PWeh / Enable pulse width and Data setup time
    bclr    LATB, #0x5      ; Set enable to low
    call    delay500us      ; Address/Data hold time and t_cycle
    clr     LATB
    return

lcd_send8bits:
    call    lcd_send4bits   ; Send upper 4 bits
    mov     W2, W1          ; Load lower 4 bits
    call    lcd_send4bits   ; Send lower 4 bits
    return

waitForButtonPress:
    btsc    PORTA, #0x0
    goto waitForButtonPress
    return

delay10s:
    mov #0x00CD, W0
    mov W0, i
    mov #0xFFFF, W0
    mov W0, j
loop10s:
    dec j
    bra nz, loop10s
    dec i
    bra nz, loop10s
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

delay500us:
    mov     #0x02C0, W0
    mov     W0, j
loop500us:
    dec     j
    bra     nz, loop500us
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


.end
