#define FCY 4000000

#include "xc.h"
#include "libpic30.h"

#define KP_COL1    _LATA1
#define KP_COL2    _LATA2
#define KP_COL3    _LATA3
#define KP_ROW1    _RB7
#define KP_ROW2    _RB8
#define KP_ROW3    _RB9
#define KP_ROW4    _RB13

#define DEBOUNCE    100

void keypad_init() {

    CNPU1 = CNPU1 | 0x2000;
    CNPU2 = CNPU2 | 0x00E1;

}

char scan_keys() {

    KP_COL2 = 0;
    KP_COL1 = KP_COL3 = 1;
    if(!KP_ROW1) { __delay_ms(DEBOUNCE); while(!KP_ROW1); return '2';}
    if(!KP_ROW2) { __delay_ms(DEBOUNCE); while(!KP_ROW2); return '5';}
    if(!KP_ROW3) { __delay_ms(DEBOUNCE); while(!KP_ROW3); return '8';}
    if(!KP_ROW4) { __delay_ms(DEBOUNCE); while(!KP_ROW4); return '0';}

    //__delay_ms(1);

    KP_COL3 = 0;
    KP_COL1 = KP_COL2 = 1;
    if(!KP_ROW1) { __delay_ms(DEBOUNCE); while(!KP_ROW1); return '3';}
    if(!KP_ROW2) { __delay_ms(DEBOUNCE); while(!KP_ROW2); return '6';}
    if(!KP_ROW3) { __delay_ms(DEBOUNCE); while(!KP_ROW3); return '9';}
    if(!KP_ROW4) { __delay_ms(DEBOUNCE); while(!KP_ROW4); return '-';}

    //__delay_ms(1);

    KP_COL1 = 0;
    KP_COL2 = KP_COL3 = 1;
    if(!KP_ROW1) { __delay_ms(DEBOUNCE); while(!KP_ROW1); return '1';}
    if(!KP_ROW2) { __delay_ms(DEBOUNCE); while(!KP_ROW2); return '4';}
    if(!KP_ROW3) { __delay_ms(DEBOUNCE); while(!KP_ROW3); return '7';}
    if(!KP_ROW4) { __delay_ms(DEBOUNCE); while(!KP_ROW4); return 21;}

    //__delay_ms(1);

    KP_COL1 = KP_COL2 = KP_COL3 = 1;

    return -1;
    
}
