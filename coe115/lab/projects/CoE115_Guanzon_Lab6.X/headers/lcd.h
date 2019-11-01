#define FCY 4000000

#include <string.h>
#include "xc.h"
#include "libpic30.h"

#define LCD_D4  _LATB0
#define LCD_D5  _LATB1
#define LCD_D6  _LATB2
#define LCD_D7  _LATB3
#define LCD_REG_SEL _LATB4
#define LCD_ENABLE  _LATB5

#define INST_REG 0
#define DATA_REG 1

#define TEXT_LEFT 0
#define TEXT_CENTER 1
#define TEXT_RIGHT 2

void lcd_write_4(int lcd_reg, int word) {

    LCD_REG_SEL = lcd_reg;
    __delay_us(500);        // t_as / Setup time
    LCD_ENABLE = 1;
    __delay_us(500);        // Data delay time
    LATB |= word;           // Copy word into LATB while masking other bits
    __delay_us(500);        // PWeh / Enable pulse width and data setup time
    LCD_ENABLE = 0;
    __delay_us(500);        // Address/Data hold time and t_cycle
    LATB = 0;

}

void lcd_write_8(int lcd_reg, int upper_word, int lower_word) {

    lcd_write_4(lcd_reg, upper_word);
    lcd_write_4(lcd_reg, lower_word);

}

void lcd_show_cursor() {
    lcd_write_8(INST_REG, 0x0, 0xE);
}

void lcd_hide_cursor() {
    lcd_write_8(INST_REG, 0x0, 0xC);
}

void lcd_cursor_first_line() {
    lcd_write_8(INST_REG, 0x0, 0x2);
}

void lcd_cursor_second_line () {
    lcd_write_8(INST_REG, 0xC, 0x0);
}

void lcd_write_character(char c) {
    int upper_word;
    int lower_word;

    // Decode character to upper and lower words
    if (c == 39) {                  // apostrophe
        upper_word = 0x2;
        lower_word = 0x7;
    } else if (c == '.') {
        upper_word = 0x2;
        lower_word = 0xE;
    } else if (c == '+') {
        upper_word = 0x2;
        lower_word = 0xB;
    } else if (c == '*') {
        upper_word = 0x2;
        lower_word = 0xA;
    } else if (c == '-') {           // dash
        upper_word = 0x2;
        lower_word = 0xD;
    } else if (c == ' ') {           // space
        upper_word = 0xA;
        lower_word = 0x0;
    } else if (c >= '0' && c <= '?') { 
        upper_word = 0x3;
        lower_word = c - '0';
    } else if (c >= '@' && c <= 'O') {
        upper_word = 0x4;
        lower_word = c - '@';
    } else if (c >= 'P' && c <= '_') {
        upper_word = 0x5;
        lower_word = c - 'P';
    } else if (c >= '`' && c <= 'o') {
        upper_word = 0x6;
        lower_word = c - '`';
    } else if (c >= 'p' && c <= '}') {
        upper_word = 0x7;
        lower_word = c - 'p';
    }

    lcd_write_8(DATA_REG, upper_word, lower_word);

}

void lcd_write_string(char string[], int alignment) {
    int i;
    int spaces;

    switch (alignment) {
        case TEXT_CENTER:
            spaces = (16 - strlen(string)) / 2;
            break;
        case TEXT_RIGHT:
            spaces = 16 - strlen(string);
            break;
    }

    for (i = 0; i < spaces; i++)
        lcd_write_character(' ');

    for (i = 0; string[i] != 0; i++)
        lcd_write_character(string[i]);

}

void lcd_clear() {
    lcd_write_8(INST_REG, 0x0, 0x1);
}

void lcd_init() {

    TRISB = 0xFFC0;

    __delay_ms(15);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    __delay_ms(5);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    __delay_us(100);
    lcd_write_4(INST_REG, 0x3);       // Function Set Command: (8-bit interface)
    lcd_write_4(INST_REG, 0x2);       // Set to 4-bit interface
    lcd_write_8(INST_REG, 0x2, 0x8);  // Set to 4-bit interface and set N and F
    lcd_write_8(INST_REG, 0x0, 0x8);  // Send Display Off command
    lcd_clear();
    lcd_write_8(INST_REG, 0x0, 0x6);  // Set Entry Mode command
    lcd_write_8(INST_REG, 0x0, 0xF);  // Send Display On command

}