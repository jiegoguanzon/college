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

#define DEBOUNCEMAX 100

void __attribute__ ((interrupt)) _CNInterrupt(void);

int rowPressed = 0;
int currColumn = 0;
int buttonPressed = 0;

int main (void) {

    AD1PCFG = 0xFFFF;
    TRISA = 0x000F;
    TRISB = 0xFE00;
    LATB = 0xFE07;

    CNPU1 = CNPU1 | 0x000C;
    CNPU2 = CNPU2 | 0x6000;
    CNEN1 = CNEN1 | 0x000C;
    CNEN2 = CNEN2 | 0x6000;

    IEC1bits.CNIE = 1;
    IFS1bits.CNIF = 0;

    _LATB0 = 1;
    _LATB1 = 1;
    _LATB2 = 0;

    while (1) {

        currColumn++; 
        if (currColumn == 4) currColumn = 1;

        switch (currColumn) {
            case 1:
                _LATB0 = 0;
                _LATB2 = 1;
                break;
            case 2:
                _LATB0 = 1;
                _LATB1 = 0;
                break;
            case 3:
                _LATB1 = 1;
                _LATB2 = 0;
                break;
        }

        if (buttonPressed) {
            switch (buttonPressed) {
                case 1:
                    _LATB4 = !_LATB4;
                    break;
                case 2:
                    _LATB5 = !_LATB5;
                    break;
                case 3:
                    _LATB7 = !_LATB7;
                    break;
                case 4:
                    _LATB8 = !_LATB8;
                    break;
            }
            rowPressed = 0;
            buttonPressed = 0;
        }

    }
    
    return 0;

}

void __attribute__ ((interrupt)) _CNInterrupt(void) {

    int debounceCounter = 0;

    if (!_RA0) {

        while (!_RA0 && (debounceCounter < DEBOUNCEMAX)) {
            debounceCounter++;
        }

        if (debounceCounter == DEBOUNCEMAX) {
            rowPressed = 1;
            buttonPressed = ((rowPressed - 1) * 3) + currColumn;
        }
        else {
            rowPressed = 0;
            buttonPressed = 0;
        }
        
    } else if (!_RA1) {

        while (!_RA1 && (debounceCounter < DEBOUNCEMAX)) {
            debounceCounter++;
        }

        if (debounceCounter == DEBOUNCEMAX) {
            rowPressed = 2;
            buttonPressed = ((rowPressed - 1) * 3) + currColumn;
        }
        else {
            rowPressed = 0;
            buttonPressed = 0;
        }
        
    }


    IFS1bits.CNIF = 0;

}