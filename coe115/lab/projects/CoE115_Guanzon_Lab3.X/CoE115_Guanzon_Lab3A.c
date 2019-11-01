/*
 * File:   CoE115_Guanzon_Lab3.c
 * Author: jiego
 *
 * Created on February 27, 2019, 8:43 AM
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

int main(void) {

    AD1PCFG = 0xFFFF;
    TRISA = 0x0002;
    TRISB = 0xFFEB;
    LATB = 0xFFFF;

    CNPU1 = CNPU1 | 0x0008;

    _LATB2 = 0;

    while (1) {

        if (!_RA1)
            _LATB4 = 1;
        else
            _LATB4 = 0;

    }

    return 0;
}
