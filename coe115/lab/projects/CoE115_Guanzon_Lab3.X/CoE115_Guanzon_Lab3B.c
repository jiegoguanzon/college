/*
 * File:   CoE115_Guanzon_Lab3B.c
 * Author: jiego
 *
 * Created on February 27, 2019, 9:48 AM
 */

#include "xc.h"

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config PLL96MHZ = OFF
#pragma config PLLDIV = NODIV
#pragma config SOSCSEL = IO

#define DEBOUNCEMAX 10

void __attribute__ ((interrupt)) _CNInterrupt(void);
void led1_toggle(void);

int row1_isPressed;

int mait (void) {

    AD1PCFG = 0xFFFF;
    TRISA = 0x0002;
    TRISB = 0xFFEB;
    LATB = 0xFFFF;

    CNPU1 = CNPU1 | 0x0008;

    CNEN1 = CNEN1 | 0x0008;
    IEC1bits.CNIE = 1;
    IFS1bits.CNIF = 0;

    _LATB2 = 0;

    while (1) {
        if (row1_isPressed) {
            led1_toggle();
            row1_isPressed = 0;
        }
    }
    
    return 0;

}

void __attribute__ ((interrupt)) _CNInterrupt(void) {

    int debounceCounter = 0;

    if (!_RA0) {

        while (!_RA1 && (debounceCounter < DEBOUNCEMAX))
            debounceCounter++;

        if (debounceCounter == DEBOUNCEMAX)
            row1_isPressed = 1;
        else
            row1_isPressed = 0;
        
    }

    IFS1bits.CNIF = 0;

}

void led1_toggle(void) {

    _LATB4 = !_LATB4;
    
}