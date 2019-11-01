#define FCY 4000000

#include "xc.h"
#include "libpic30.h"

#define LCD_REG_SEL _LATB4
#define LCD_ENABLE _LATB5
#define INST_REG 0
#define DATA_REG 1

#define TEXT_LEFT 0
#define TEXT_CENTER 1
#define TEXT_RIGHT 2

void lcd_write_4(int lcd_reg, int word) {

    __delay_ms(1000);
    //LCD_REG_SEL = lcd_reg;
    _LATB4 = 0;
    __delay_us(500);        // t_as / Setup time
    __delay_ms(15);
    __delay_ms(1000);
    //LCD_ENABLE = 1;
    _LATB5 = 1;
    __delay_us(500);        // Data delay time
    __delay_ms(15);
    __delay_ms(1000);
    LATB |= word;           // Copy word into LATB while masking other bits
    __delay_us(500);        // PWeh / Enable pulse width and data setup time
    __delay_ms(15);
    __delay_ms(1000);
    //LCD_ENABLE = 0;
    _LATB5 = 0;
    __delay_us(500);        // Address/Data hold time and t_cycle
    __delay_ms(15);
    __delay_ms(1000);
    LATB = 0;
    __delay_ms(15);
    __delay_ms(1000);

    return 0;
}

void lcd_write_8(int lcd_reg, int upper_word, int lower_word) {

    lcd_write_4(lcd_reg, upper_word);
    lcd_write_4(lcd_reg, lower_word);

    return 0;
}

void lcd_cursor_first_line() {
    lcd_write_8(INST_REG, 0x0, 0x2);
    return 0;
}

void lcd_cursor_second_line () {
    lcd_write_8(INST_REG, 0xC, 0x0);
    return 0;
}

void lcd_write_character(char c) {
    int upper_word;
    int lower_word;

    // Decode character to upper and lower words
    if (c > 47 && c < 64) {
        upper_word = 0x3;
        lower_word = c - 48;
    } else if (c > 63 && c < 80) {
        upper_word = 0x4;
        lower_word = c - 64;
    } else if (c > 79 && c < 96) {
        upper_word = 0x5;
        lower_word = c - 80;
    } else if (c > 96 && c < 112) {
        upper_word = 0x6;
        lower_word = c - 97;
    } else if (c > 112 && c < 126) {
        upper_word = 0x7;
        lower_word = c - 113;
    }

    lcd_write_8(DATA_REG, upper_word, lower_word);

    return 0;
}

void lcd_write_string(char string[]) {
    int i;
    for (i = 0; string[i] != 0; i++) {
        lcd_write_character(string[i]);
    }

    return 0;
}

void lcd_clear() {
    lcd_write_8(INST_REG, 0x0, 0x1);
    return 0;
}

void lcd_init() {

    TRISB = 0xFFC0;

    __delay_ms(15);
    __delay_ms(1000);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    __delay_ms(5);
    __delay_ms(15);
    __delay_ms(1000);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    __delay_us(100);
    __delay_ms(1000);
    __delay_ms(15);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    lcd_write_4(INST_REG, 0x2);       // Set to 4-bit interface
    lcd_write_8(INST_REG, 0x2, 0x8);  // Set to 4-bit interface and set N and F
    lcd_write_8(INST_REG, 0x0, 0x8);  // Send Display Off command
    lcd_clear();
    lcd_write_8(INST_REG, 0x0, 0x6);  // Set Entry Mode command
    lcd_write_8(INST_REG, 0x0, 0xF);  // Send Display On command

    return 0;
}