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

long map(long input, long in_min, long in_max, long out_min, long out_max); 
void ADC_init(void);
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void);

long adcvalue; 
int duty_cycle = 0;
int prev_duty_cycle = 0;

int main(void) {

    ADC_init();

    TRISA = 0xFFF0;
    TRISB = 0xFFC0;

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

    while(1) {
        duty_cycle = map(adcvalue, 0, 1023, (long) (147 * 0.1), (long) (147 * 0.9));
        if(prev_duty_cycle != duty_cycle) {
            OC2R = duty_cycle;
            prev_duty_cycle = duty_cycle;
        }
    }

    return 0;

}

long map(long input, long in_min, long in_max, long out_min, long out_max) {
    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ADC_init() {

    AD1PCFG = 0xFDFF;
    AD1CON1 = 0x20E7;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0201;
    //AD1CON3 = 0x1FFF;
    AD1CHS = 0x0009;
    AD1CSSL = 0x0000;
    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;
    AD1CON1bits.ADON = 1;

}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    IEC0bits.AD1IE = 0;
    IFS0bits.AD1IF = 0;
    adcvalue = ADC1BUF0;
    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;
}

