#define FCY 4000000

#include "xc.h"
#include "libpic30.h"

void UART_init() {

    U1BRG = 25; // 9600 Baud Rate

    _U1TXIP2 = 1;
    _U1TXIP1 = 0;
    _U1TXIP0 = 0;
    _U1RXIP2 = 1;
    _U1RXIP1 = 0;
    _U1RXIP0 = 0;

    U1STA = 0;  // Clear status register
    U1MODE = 0x8000; // Enable 8-bit mode

    /* Assign UART TX and RX to remappable pins */
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    _U1RXR = 2; // UART1 RX to RB2/RP2
    _RP1R = 3;  // UART1 TX to RB1/RP1
    __builtin_write_OSCCONL(OSCCON | 0x40);

    _UTXEN = 1; // Enable transmit

    _U1TXIE = 1;  // Enable transmit interrupt
    _U1TXIF = 0;  // Clear transmit interrupt flag

    _U1RXIE = 1;  // Enable receive interrupt
    _U1RXIF = 0;  // Clear receive interrupt flag

}

void UART_send_byte(char d) {
    while (!_TRMT) ;
    //while (_UTXBF == 1) ;
    U1TXREG = d;
}

void UART_send_string(char *str) {
    uint8_t i;
    for (i = 0; str[i] != '\0'; i++)
        UART_send_byte(str[i]);
}
