/*
 * File:   CoE115_Guanzon_Lab5.c
 * Author: jiego
 *
 * Created on March 13, 2019, 10:26 AM
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

    TRISA = 0xFFF0;

    OC2CON1 = 0;
    OC2CON2 = 0;

    OC2CON1bits.OCTSEL = 0x07;
    OC2CON1bits.OCM = 0x06;
    OC2CON2bits.SYNCSEL = 0x1F;

    __builtin_write_OSCCONL(OSCCON & 0xBF);

    _RP6R = 19;

    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC2R = 147 / 2;
    OC2RS = 147;

    while(1);

    return 0;

}
