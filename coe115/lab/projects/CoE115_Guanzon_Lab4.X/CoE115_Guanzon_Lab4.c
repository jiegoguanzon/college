/*
 * File:   CoE115_Guanzon_Lab4.c
 * Author: jiego
 *
 * Created on March 13, 2019, 12:43 AM
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

int map(int input, int in_min, int in_max, int out_min, int out_max); 
void ADC_init(void);
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void);

int adcvalue; 

int led_count;
int prev_led_count;

int main(void) {

    ADC_init();

    IEC0bits.AD1IE = 1;
    IFS0bits.AD1IF = 0;

    AD1CON1bits.ADON = 1;

    while (1) {

        led_count = map(adcvalue, 0, 1023, 0, 6);

        if (prev_led_count != led_count) {

            LATB = LATB & 0xFF80;

            switch (led_count) {
                case 6:
                    _LATB5 = 1;
                case 5:
                    _LATB4 = 1;
                case 4:
                    _LATB3 = 1;
                case 3:
                    _LATB2 = 1;
                case 2:
                    _LATB1 = 1;
                case 1:
                    _LATB0 = 1;
                    break;
                default:
                    LATB = LATB & 0xFF80;
                    break;
            }

            prev_led_count = led_count;

        }

    }

    return 0;

}

int map(int input, int in_min, int in_max, int out_min, int out_max) {
    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ADC_init() {

    AD1PCFG = 0xFDFF;
    AD1CON1 = 0x20E7;
    AD1CON2 = 0x0000;
    AD1CON3 = 0x0201;
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
