/*
 * File:   CoE115_Guanzon_Lab6.c
 * Author: jiego
 *
 * Created on April 30, 2019, 9:04 AM
 */

#include "xc.h"
#include "headers/uart.h"
#include "headers/lcd_i2c.h"
#include "headers/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config PLL96MHZ = OFF
#pragma config PLLDIV = NODIV
#pragma config SOSCSEL = IO

#define DEBOUNCE_DELAY 100

#define LCD_COL_COUNT 20
#define LCD_ROW_COUNT 4

#define BOAT_COUNT  4

#define LCD_WALL        0
#define VERT_BOAT_TOP   1
#define VERT_BOAT_BOT   2
#define HORI_BOAT_LEFT  3
#define HORI_BOAT_RIGHT 4
#define BOAT_BODY       5

#define STR_LEN     5

#define HORIZONTAL  0
#define VERTICAL    1

#define UP_PB       _RA0
#define DOWN_PB     _RA1
#define LEFT_PB     _RA2
#define RIGHT_PB    _RA3

#define A_PB     _RB5
#define B_PB     _RB7

#define WATER   0
#define BOAT    1
#define FLAG    2
#define WRECK   3
#define WALL    4
#define Time_Limit 5

uint8_t count = 0;
char c = 0;
char str[STR_LEN];

uint8_t proceed;
uint8_t boat_size;
uint8_t boat_orientation;
uint8_t curr_col, curr_row;
uint8_t prev_col, prev_row;
uint8_t data_is_available;
uint8_t a_pb_flag, b_pb_flag;
uint8_t game_end = 0;
uint8_t Winner = 0;
uint8_t timer = 0;
uint8_t enable = 1;
int battlefield[20][4];

int count_remaining_boats (void);

void __attribute__ ((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    _U1TXIF = 0;
}

void __attribute__ ((interrupt, no_auto_psv)) _U1RXInterrupt(void) {

    if (!count)
        memset(str, 0, STR_LEN);

    c = U1RXREG;
    if (c == '|') {
        Winner = 1;
        game_end = 1;
    }
    
    str[count] = c; 
    count++;

    if (count == STR_LEN) count = 0;
    


    data_is_available = 1;
    _U1RXIF = 0;

}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt (void) {
    _T1IE = 0;
    _T1IF = 0;
    
    timer++;
    
    if (timer >= Time_Limit){
        enable = 1;
        timer = 0;
    }
    
    _T1IE = 1;
    _T1IF = 0;
}

void T1_init(){
    T1CON = 0x8030;
    _T1IF = 0;
    _T1IP = 6;
    PR1 = 15625;
    _T1IE = 1;
    return;
}

void print_battlefield() {
    
    uint8_t i, j;

    for (j = 0; j < LCD_ROW_COUNT; j++) {
        LCD_position_cursor(1, j + 1);
        for (i = 0; i < 20; i++)
            LCD_send_character(battlefield[i][j] + 48);
    }

}

int place_boat (uint8_t size, uint8_t orientation) {

    uint8_t i;

    for (i = 0; i < size; i++) {

        if (orientation == HORIZONTAL) {
            if (battlefield[curr_col - 1 + i][curr_row - 1] == BOAT)
                return 0;
        } else {
            if (battlefield[curr_col - 1][curr_row - 1 + i])
                return 0;
        }

    }

    for (i = 0; i < size; i++) {

        if (i == 0) {
            if (orientation == HORIZONTAL)
                LCD_send_character(HORI_BOAT_LEFT);
            else 
                LCD_send_character(VERT_BOAT_TOP);
        } else if (i == size - 1) {
            if (orientation == HORIZONTAL)
                LCD_send_character(HORI_BOAT_RIGHT);
            else 
                LCD_send_character(VERT_BOAT_BOT);
        } else 
            LCD_send_character(BOAT_BODY);

        if (a_pb_flag)
            battlefield[curr_col - 1][curr_row - 1] = BOAT;

        if (orientation == VERTICAL) {
            if (curr_row != LCD_ROW_COUNT) ++curr_row;
            LCD_position_cursor(curr_col, curr_row);
        } else if (curr_col != LCD_COL_COUNT)
            curr_col++;

    }

    return 1;

}

int poll_buttons (uint8_t min_x, uint8_t max_x, uint8_t min_y, uint8_t max_y) {

    if (A_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (A_PB) ;
        a_pb_flag = 1;
        return 1;
    } else if (B_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (B_PB) ;
        b_pb_flag = 1;
        return 1;
    }

    if (UP_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (UP_PB) ;
        prev_row = curr_row;
        if (curr_row != min_y) --curr_row;
    } else if (DOWN_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (DOWN_PB) ;
        prev_row = curr_row;
        if (curr_row != max_y) ++curr_row;
    } else if (RIGHT_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (RIGHT_PB) ;
        prev_col = curr_col;
        if (curr_col != max_x) ++curr_col;
    } else if (LEFT_PB) {
        __delay_us(DEBOUNCE_DELAY);
        while (LEFT_PB) ;
        prev_col = curr_col;
        if (curr_col != min_x) --curr_col;
    } else
        return 0;

    LCD_position_cursor(curr_col, curr_row);

    return 1;

}

void position_boats() {

    uint8_t i, j;

    uint8_t min_x = 1;
    uint8_t max_x = 9;

    uint8_t min_y = 1;
    uint8_t max_y = 4;

    curr_col = prev_col = 1;
    curr_row = prev_row = 1;
    a_pb_flag = b_pb_flag = 0;

    boat_orientation = VERTICAL;

    // Initialize battlefield array
    for (i = 0; i < LCD_COL_COUNT; i++)
        for (j = 0; j < LCD_ROW_COUNT; j++)
            battlefield[i][j] = WATER;

    // Draw divider onto LCD and array
    for (i = 0; i < LCD_ROW_COUNT; i++) {
        LCD_position_cursor(LCD_COL_COUNT / 2, 1 + i);
        //LCD_send_string("||");
        LCD_send_character(0);
        LCD_send_character(0);
        battlefield[LCD_COL_COUNT / 2 - 1][i] = WALL;
        battlefield[LCD_COL_COUNT / 2][i] = WALL;
    }


    boat_size = 4;
    LCD_position_cursor(13, 1);
    LCD_send_string("Size: ");
    LCD_send_character(boat_size + 48);

    LCD_position_cursor(13, 2);
    LCD_send_string("Orient: ");
    LCD_position_cursor(12, 3);
    if (boat_orientation == HORIZONTAL)
        LCD_send_string("HORIZONTL");
    else 
        LCD_send_string(" VERTICAL");

    LCD_position_cursor(1, 1);

    // Place boats
    for (i = 0; i < BOAT_COUNT; i++) {

        while (a_pb_flag == 0 && b_pb_flag == 0) {
            poll_buttons(min_x, max_x, min_y, max_y);
        }

        if (a_pb_flag) {

            if (place_boat(boat_size, boat_orientation) == 0)
                continue;

            if (boat_size != 2) boat_size--;

            LCD_position_cursor(19, 1);
            LCD_send_character(boat_size + 48);
            LCD_position_cursor(curr_col, curr_row);

            a_pb_flag = 0;

        } else if (b_pb_flag) {

            boat_orientation = !boat_orientation;
            i--;

            LCD_position_cursor(12, 3);
            if (boat_orientation == HORIZONTAL)
                LCD_send_string("HORIZONTL");
            else 
                LCD_send_string(" VERTICAL");
            LCD_position_cursor(curr_col, curr_row);

            b_pb_flag = 0;

        }

    }

}

void game_proper() {

    data_is_available = 0;

    uint8_t col = 0;
    uint8_t row = 0;

    uint8_t min_x = 12;
    uint8_t max_x = 20;

    uint8_t min_y = 1;
    uint8_t max_y = 4;

    char data[5];

    curr_col = min_x;
    curr_row = min_y;
    LCD_position_cursor(curr_col, curr_row);

    LCD_send_string("         ");
    LCD_position_cursor(min_x, min_y + 1);
    LCD_send_string("         ");
    LCD_position_cursor(min_x, min_y + 2);
    LCD_send_string("         ");
    LCD_position_cursor(min_x, min_y + 3);
    LCD_send_string("         ");
    LCD_position_cursor(curr_col, curr_row);

    count = 0;
    memset(str, 0, 6);

    while (game_end != 1) {

        while (a_pb_flag == 0 && b_pb_flag == 0 && data_is_available == 0) {
            poll_buttons(min_x, max_x, min_y, max_y);
        }

        if (data_is_available) {

            while (strlen(str) < STR_LEN) ;

            data_is_available = 0;

            if (!strcmp(str, "HIT!!")) {

                col += 10;
                row += -1;

                LCD_send_character('X');
                if (curr_col != max_x) curr_col++;
                LCD_position_cursor(curr_col, curr_row);
                battlefield[col][row] = WRECK;

            } else if (!strcmp(str, "MISS!")) {

                LCD_send_character('O');
                if (curr_col != max_x) curr_col++;
                LCD_position_cursor(curr_col, curr_row);

            } else {

                col = str[1] - 1 - 48;
                row = str[3] - 1 - 48;

                if (battlefield[col][row] == BOAT) {
                    battlefield[col][row] = WRECK;
                    LCD_position_cursor(col + 1, row + 1);
                    LCD_send_character('X');
                    LCD_position_cursor(curr_col, curr_row);
                    UART_send_string("HIT!!");
                } else if (battlefield[col][row] == WATER) {
                    UART_send_string("MISS!");
                }

            }


        } else if (a_pb_flag && enable) {
            enable = 0;
            timer = 0;
            // Send curr_col and curr_row to other GameBaby

            col = curr_col - 11;
            row = curr_row;

            sprintf(data, "(%d,%d)", col, row);
            UART_send_string(data);

            a_pb_flag = 0;

        } else if (b_pb_flag) {

            col = curr_col - 1;
            row = curr_row - 1;

            // Place or unplace FLAG

            if (battlefield[col][row] == FLAG) {

                LCD_send_character(' ');
                if (curr_col != max_x) curr_col++;
                battlefield[col][row] = WATER;

            } else if (battlefield[col][row] == WATER) {

                LCD_send_character('!');
                if (curr_col != max_x) curr_col++;
                battlefield[col][row] = FLAG;

            }

            b_pb_flag = 0;

        }
        if (count_remaining_boats() == 0) {
            UART_send_byte('|');
            Winner = 0;
            game_end = 1;
        }
    }

}

void game_start (void) {
    
    LCD_hide_cursor();
    LCD_cursor_second_line () ;
    LCD_send_string ("     WELCOME TO     ") ;
    LCD_cursor_third_line () ;
    LCD_send_string ("     BATTLESHIP    ") ;
    __delay_ms (2000) ;
    LCD_clear();
    
    LCD_cursor_second_line () ;
    LCD_send_string ("TRYING TO CONNECT TO") ;
    LCD_cursor_third_line () ;
    LCD_send_string ("  ANOTHER GAMEBABY" ) ;

    int i1 = 0;

    while (1) {

        LCD_send8bits (0xD4 + 8, INST_REG) ;

        switch (i1 % 4){
            case 0: 
                LCD_send_string(".    ");
                break;
            case 1: 
                LCD_send_string("..  ");
                break;
            case 2: 
                LCD_send_string("... ");
                break;
            case 3: 
                LCD_send_string("....");
                break;
        }

        UART_send_byte('@');

        if (c == '@') {
            __delay_ms(260);
            UART_send_byte('@');
            break;
        }

        i1++;

        __delay_ms(250);

    }

    LCD_clear () ;
    LCD_cursor_second_line () ;
    LCD_send_string ("     CONNECTED!     ") ;
    LCD_cursor_third_line () ;
    LCD_send_string ("   READY TO PLAY!  ") ;

    __delay_ms (3000) ;

    LCD_clear () ;
    LCD_show_cursor () ;

}

int count_remaining_boats (void) {
    int i, j, k = 0;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 20; j++){
            if (battlefield[j][i] == BOAT){
                k++;
            }
        }
    }
    return k;
}

void end_game(void){
    LCD_clear();
    LCD_hide_cursor();
    if (Winner){
        LCD_cursor_second_line();
        LCD_send_string("  *CONGRATULATIONS*  ");
        LCD_cursor_third_line();
        LCD_send_string("YOU ARE THE WINNER!!");
    }else {
        LCD_cursor_first_line();
        LCD_send_string("       SORRY!       ");
        LCD_cursor_second_line();
        LCD_send_string("      YOU LOST      ");
        LCD_cursor_third_line();
        LCD_send_string("    BETTER LUCK,    ");
        LCD_cursor_fourth_line();
        LCD_send_string("     NEXT TIME!     ");
    }
    __delay_ms(5000);
    asm("RESET");
}

int main(void) {

    AD1PCFG = 0xFFFF;
    TRISB = 0xFFFF;
    TRISA = 0xFFFF;

    LCD_init();
    UART_init();
    
    game_start () ;

    // Wait until a connection with another GameBaby is estabished
    
    LCD_store_character(0, c_wall);
    LCD_store_character(8, c_vert_top);
    LCD_store_character(16, c_vert_bot);
    LCD_store_character(24, c_hori_left);
    LCD_store_character(32, c_hori_right);
    LCD_store_character(40, c_body);

    position_boats();

    UART_send_byte(1);

    while (!data_is_available) ;

    while (proceed != 1) {
        proceed = c;
    }
    
    data_is_available = 0;
    T1_init();
    game_proper () ;
    end_game () ;
    while (1) ;

    return 0;

}  