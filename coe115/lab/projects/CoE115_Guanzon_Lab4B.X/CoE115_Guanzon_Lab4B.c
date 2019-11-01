/*
 * File:   CoE115_Guanzon_Lab4B.c
 * Author: jiego
 *
 * Created on March 13, 2019, 9:14 AM
 */

#define FCY 4000000

#include "xc.h"
#include "libpic30.h"

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
int increment = -1;
int curr_led = 1;

int main(void) {

    ADC_init();

    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;

    AD1CON1bits.ADON = 1;

    while (1) {

        if(curr_led == 1 || curr_led == 6) increment *= -1;

        LATB = LATB & 0xFF80;

        switch (curr_led) {
            case 1:
                _LATB0 = 1;
                break;
            case 2:
                _LATB1 = 1;
                break;
            case 3:
                _LATB2 = 1;
                break;
            case 4:
                _LATB3 = 1;
                break;
            case 5:
                _LATB4 = 1;
                break;
            case 6:
                _LATB5 = 1;
                break;
        }

        curr_led += increment;

        __delay_ms(map(adcvalue, 0, 1023, 125, 1000));

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
    //AD1CON3 = 0x0201;
    AD1CON3 = 0x1FFF;
    AD1CHS = 0x0009;
    AD1CSSL = 0x0000;
    TRISB = 0xFFC0;

}

void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void) {
    IEC0bits.AD1IE = 0;
    IFS0bits.AD1IF = 0;
    adcvalue = ADC1BUF0;
    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;
}
