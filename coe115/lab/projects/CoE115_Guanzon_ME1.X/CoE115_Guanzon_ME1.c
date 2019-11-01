/*
 * File:   CoE115_Guanzon_ME1.c
 * Author: jiego
 *
 * Created on March 20, 2019, 2:50 PM
 */

#include "lcd.h"

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config PLL96MHZ = OFF
#pragma config PLLDIV = NODIV
#pragma config SOSCSEL = IO

#define PB _RA0

void game_start(void);
void mode_select(void);
void adc_init(void);
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void);
long map(long input, long in_min, long in_max, long out_min, long out_max); 

int lcd_data;

long adcvalue; 

int main(void) {
    
    //adc_init();
    AD1PCFG = 0xFFFF;
    TRISB = 0xFFC0;
    LATB = 0;


    lcd_init();

    //game_start();
    //mode_select();

    while (1) ;

    return 0;

}

void mode_select() {
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("GAME MODE");
    lcd_cursor_second_line();
    lcd_write_string("SELECT");
    __delay_ms(1000);
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("1-Key Input Mode");
    lcd_cursor_second_line();
    lcd_write_string("2-Multi Choice");
    while(1);
}

void wait_for_pushbutton() {
    while (PB == 0);
}

void game_start() {
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("WELCOME TO");
    lcd_cursor_second_line();
    lcd_write_string("JIEGO'S GAME");
    wait_for_pushbutton();
}

void adc_init() {
    AD1PCFG = 0xFDFF;
    AD1CON1 = 0x20E7;
    AD1CON2 = 0x0000;
    //AD1CON3 = 0x0201;
    AD1CON3 = 0x1FFF;
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

long map(long input, long in_min, long in_max, long out_min, long out_max) {
    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}