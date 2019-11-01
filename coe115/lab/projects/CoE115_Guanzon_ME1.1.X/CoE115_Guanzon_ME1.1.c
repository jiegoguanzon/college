/*
 * File:   CoE115_Guanzon_ME1.c
 * Author: jiego
 *
 * Created on March 20, 2019, 2:50 PM
 */

#include "headers/lcd.h"
#include "headers/keypad.h"
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

#define PB _RA0

void wait_for_pushbutton();
char get_operation(int operation);
void key_input_mode();
void multiple_choice_mode();
char * get_difficulty();
void diff_select(void);
void mode_select(void);
void game_start(void);
void adc_init(void);
void __attribute__ ((interrupt, no_auto_psv)) _ADC1Interrupt(void);
void __attribute__ ((interrupt)) _CNInterrupt(void);
long map(long input, long in_min, long in_max, long out_min, long out_max); 

int pb_is_pressed = 0;

int lcd_data;

long adcvalue; 

int game_mode;

int difficulty;
int prev_difficulty;

int score;

int operand_a;
int operand_b;
int operation;
int op_answer;

int main(void) {
    
    TRISA = 0xFFF1;
    TRISB = 0xFFC0;
    //LATA = 0;
    LATB = 0;

    adc_init();
    lcd_init();
    keypad_init();

    CNEN1 = CNEN1 | 0x0004;
    IEC1bits.CNIE = 1;
    IFS1bits.CNIF = 0;

    score = 0;

    game_start();
    mode_select();
    diff_select();
    if(game_mode == 1) key_input_mode();
    else multiple_choice_mode();

    while (1) ;

    return 0;

}

char get_operation(int operation) {
    switch (operation) {
        case 0:
            return '+';
        case 1:
            return '-';
        case 2:
            return '*';
    }
    return '+';
}

void get_expression() {

    srand(adcvalue);
    operand_a = rand() % 10;
    operand_b = rand() % 10;
    operation = rand() % (difficulty + 1);

    switch(operation) {
        case 0:
            op_answer = operand_a + operand_b;
            break;
        case 1:
            op_answer = operand_a - operand_b;
            break;
        case 2:
            op_answer = operand_a * operand_b;
            break;
    }

    lcd_write_character(operand_a + 48);
    lcd_write_character(' ');
    lcd_write_character(get_operation(operation));
    lcd_write_character(' ');
    lcd_write_character(operand_b + 48);

}

void multiple_choice_mode() {

    char answer_str[17];
    int choice_a = 0;
    int choice_b = 0;
    int choice_c = 0;
    int choice = 0;
    int position = 0;

    while (1) {

        lcd_clear();
        lcd_cursor_first_line();

        lcd_write_string("Q: ", TEXT_LEFT);
        get_expression();

        srand(adcvalue);
        position = rand() % 3;

        switch (position) {
            case 0:
                choice_a = op_answer;
            break;
            case 1:
                choice_b = op_answer;
            break;
            case 2:
                choice_c = op_answer;
            break;
        }


        while (!choice_a || !choice_b || !choice_c) {
            srand(adcvalue);
            position = rand() % 3;
            srand(adcvalue);
            choice = rand() % 10;

            if (choice == choice_a || choice == choice_b || choice == choice_c)
                continue;

            switch (position) {
                case 0:
                    choice_a = choice;
                break;
                case 1:
                    choice_b = choice;
                break;
                case 2:
                    choice_c = choice;
                break;
            }

        }

        memset(answer_str, 0, 17);

        lcd_cursor_second_line();

        lcd_write_character((choice_a / 10) + 48);
        lcd_write_character((choice_a % 10) + 48);
        lcd_write_character(' ');
        lcd_write_character((choice_b / 10) + 48);
        lcd_write_character((choice_b % 10) + 48);
        lcd_write_character(' ');
        lcd_write_character((choice_c / 10) + 48);
        lcd_write_character((choice_c % 10) + 48);

        //lcd_write_string(answer_str, TEXT_LEFT);

        while (!pb_is_pressed) ;

    }

    while (1) ;    

}

void key_input_mode() {
    int user_answer;
    int is_negative;
    char answer_str[7] = "A: ";

    while (1) {

        lcd_clear();
        lcd_cursor_first_line();

        lcd_write_string("Q: ", TEXT_LEFT);
        get_expression();

        lcd_cursor_second_line();
        lcd_show_cursor();
        lcd_write_string("      ", TEXT_LEFT);
        lcd_cursor_second_line();
        memset(answer_str, 0, 7);
        strcpy(answer_str, "A: ");
        lcd_write_string(answer_str, TEXT_LEFT);

        int key = -1;
        int key_count = 0;

        while (!pb_is_pressed) {
            key = scan_keys();

            if (key == 21 && key_count > 0) {
                lcd_cursor_second_line();
                lcd_write_string("      ", TEXT_LEFT);
                answer_str[2 + key_count] = '\0';
                key_count--;
                lcd_cursor_second_line();
                lcd_write_string(answer_str, TEXT_LEFT);
            } else if(key != 21 && key != -1 && key_count < 3) {
                answer_str[3 + key_count] = key;
                key_count++;
                lcd_cursor_second_line();
                lcd_write_string(answer_str, TEXT_LEFT);
            }
        }

        if(answer_str[3] == '-') {
            strncpy(answer_str, answer_str + 4, 3);
            is_negative = 1;
        }
        else {
            strncpy(answer_str, answer_str + 3, 4);
            is_negative = 0;
        }

        user_answer = atoi(answer_str);

        if (is_negative && ((user_answer * -1) == op_answer))
            score += difficulty + 1;
        else if (!is_negative && (user_answer == op_answer))
            score += difficulty + 1;

        lcd_clear();
        lcd_cursor_first_line();
        //lcd_write_string(answer_str, TEXT_LEFT);
        lcd_write_character((user_answer / 10) + 48);
        lcd_write_character((user_answer % 10) + 48);
        lcd_write_character(' ');
        lcd_write_character((abs(op_answer) / 10) + 48);
        lcd_write_character((abs(op_answer) % 10) + 48);
        lcd_write_character(' ');
        lcd_write_character((score / 10) + 48);
        lcd_write_character((score % 10) + 48);

        __delay_ms(500);

        while (!pb_is_pressed) ;

    }

    while (1) ; 

}

char * get_difficulty() {
    switch (difficulty) {
        case 0:
        case 1:
            return "EASY";
        case 2:
        case 3:
            return "AVE ";
        case 4:
        case 5:
            return "HARD";
    }
    return "EASY";
}

void diff_select() {

    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("Game Mode ", TEXT_LEFT);
    lcd_write_character(game_mode + 48);

    difficulty = 0;
    prev_difficulty = -1;

    do {
        difficulty = map(adcvalue, 0, 1023, 0, 5);
        if (difficulty != prev_difficulty) {
            lcd_cursor_second_line();
            lcd_write_string("Difficulty: ", TEXT_LEFT);
            lcd_write_string(get_difficulty(), TEXT_LEFT);
            prev_difficulty = difficulty;
        }
    } while (PB == 0);

    difficulty = prev_difficulty / 2;

}

void mode_select() {
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("GAME MODE", TEXT_CENTER);
    lcd_cursor_second_line();
    lcd_write_string("SELECT", TEXT_CENTER);
    __delay_ms(1000);
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("1-Key Input Mode", TEXT_LEFT);
    lcd_cursor_second_line();
    lcd_write_string("2-Multi Choice", TEXT_LEFT);

    int key = -1;

    while(1) {
        key = scan_keys();
        if(key == '1' || key == '2') {
            game_mode = key - '0';
            break;
        }
    }
    
}

void wait_for_pushbutton() {
    while (!pb_is_pressed);
}

void game_start() {
    lcd_clear();
    lcd_cursor_first_line();
    lcd_write_string("WELCOME TO", TEXT_CENTER);
    lcd_cursor_second_line();
    lcd_write_string("JIEGO'S GAME", TEXT_CENTER);
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

void __attribute__ ((interrupt)) _CNInterrupt(void) {

    if (PB) {
        __delay_ms(100);
        if (PB) 
            pb_is_pressed = 1;
    } else {
        pb_is_pressed = 0;
    }

    IFS1bits.CNIF = 0;

}

long map(long input, long in_min, long in_max, long out_min, long out_max) {
    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}