/*
    Author: Jiego Guanzon
    Date: January 23 2019
*/

.include "p24fj64gb002.inc"
.global __reset

.bss
i: .space 1 /* Allocates 1 byte of space to the variable */
 
.text
__reset:
    
    mov #__SP_init, W15
    mov #__SPLIM_init, W0
    mov W0, SPLIM
    
    clr.b i
    mov	    #3, W3
    mov	    W3, i
    
initializeOuterCounter:
    mov	    #64, W2
    
initializeInnerCounter:
    mov	    #31249, W1

innerLoop:    
    dec	    W1, W1
    bra	    z, outerLoop
    goto    innerLoop

outerLoop:
    dec	    W2, W2
    bra	    z, timerDecrement
    goto    initializeInnerCounter
    
timerDecrement:
    dec.b   i	/* 8-bit decrement */
    goto    initializeOuterCounter
    
.end
    