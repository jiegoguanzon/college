/*
 * File:   CoE115_Guanzon_Lab6.c
 * Author: jiego
 *
 * Created on April 10, 2019, 9:15 AM
 */


#include "xc.h"
#include "headers/lcd.h"

#pragma config FWDTEN = OFF
#pragma config JTAGEN = OFF
#pragma config POSCMOD = NONE
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = FRCPLL
#pragma config PLL96MHZ = OFF
#pragma config PLLDIV = NODIV
#pragma config SOSCSEL = IO

#define READ_ADDR 0x0B
#define WRITE_ADDR 0x0F

#define EEPROM_SLAVE_ADDR 0xA0

// Initialize I2C module
void I2C_init(void) {	

  // Set values for the ff:
  // I2C1BRG
  I2C1BRG = 0x04;
  // I2C1CON
  I2C1CON = 0x2000;
  // Enable I2C after configuration
  _I2CEN = 1;

}

// Send start bit sequence
void I2C_start(void) {

  // Set Start Condition Enabled Bit
  _SEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (!_S) ;
  // Clear interrupt flag
  _MI2C1IF = 0;

}

// Send repeated start bit sequence
void I2C_restart(void) {

  // Set Repeated Start Condition Enabled bit
  _RSEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (!_S) ;
  // Clear interrupt flag
  _MI2C1IF = 0;

}

// Send stop bit sequence
void I2C_stop(void) {

  // Set Stop Condition Enabled bit
  _PEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (!_P) ;
  // Clear interrupt flag
  _MI2C1IF = 0;

}

// Send ACK bit sequence
void I2C_ack(void) {

  // Set Acknowledge Data bit
  _ACKDT = 0;
  // Set Acknowledge Sequence Enabled bit
  _ACKEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (_MI2C1IF == 0) ;
  // Clear interrupt flag
  _MI2C1IF = 0;

}

// Send NACK bit sequence
void I2C_nack(void) {

  // Set Acknowledge Data bit
  _ACKDT = 1;
  // Set Acknowledge Sequence Enabled bit
  _ACKEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (_MI2C1IF == 0) ;
  // Clear interrupt flag
  _MI2C1IF = 0;

}

// Transfer one byte sequence
int I2C_write(unsigned char byte) {

  // Write byte to transmit buffer register
  I2C1TRN = byte;
  // Wait for operation to finish by checking interrupt flag
  while (_TRSTAT) ;
  // Clear interrupt flag
  _MI2C1IF = 0;
  // Return Acknowledge bit received
  return _ACKSTAT;

}

// Receive one byte sequence
unsigned char I2C_read(void) {

  // Set Receive Enable bit
  _RCEN = 1;
  // Wait for operation to finish by checking interrupt flag
  while (!_RBF) ;
  // Clear interrupt flag
  _MI2C1IF = 0;
  // Return receive buffer contents
  return I2C1RCV;
  
}

// EEPROM write sequence
void EEPROM_write(unsigned int waddr, unsigned char wdata) {

  // Start I2C communication
  I2C_start();

  // Send I2C EEPROM slave address with write command, wait if device is not available
  while(I2C_write(EEPROM_SLAVE_ADDR + 0)){
    I2C_restart();
  }

  // Send address, high byte then low byte
  I2C_write(waddr>>8);
  I2C_write(waddr);

  // Send data
  I2C_write(wdata);

  // Stop I2C communication
  I2C_stop();

}

// EEPROM read sequence
unsigned char EEPROM_read(unsigned int raddr) {

  unsigned char byte = 0;

  // Start I2C communication
  I2C_start();

  // Send I2C EEPROM slave address with write command, wait if device is not available
  while(I2C_write(EEPROM_SLAVE_ADDR + 0)) {

    I2C_restart();

  }

  // Send address, high byte then low byte
  I2C_write(raddr>>8);
  I2C_write(raddr);

  // Send repeated start
  I2C_restart();

  // Send I2C EEPROM slave address with read command
  I2C_write(EEPROM_SLAVE_ADDR + 1);

  // Store data from buffer
  byte = I2C_read();

  // Send NACK
  I2C_nack();

  // Stop I2C communication
  I2C_stop();

  return byte;

}

int main(void) {

    int i;

    AD1PCFG = 0xFFFF;
    // Initialize I2C peripheral
    I2C_init();
    // Initialize LCD
    lcd_init();

    // Read 16 bytes from READ_ADDR
    // Display 16 bytes on LCD

    for (i = 0; i < 16; i++)
      lcd_write_character(EEPROM_read(READ_ADDR + i));

    // Write student number to WRITE_ADDR

    EEPROM_write(WRITE_ADDR, '2');
    EEPROM_write(WRITE_ADDR + 1, '0');
    EEPROM_write(WRITE_ADDR + 2, '1');
    EEPROM_write(WRITE_ADDR + 3, '5');
    EEPROM_write(WRITE_ADDR + 4, '0');
    EEPROM_write(WRITE_ADDR + 5, '7');
    EEPROM_write(WRITE_ADDR + 6, '9');
    EEPROM_write(WRITE_ADDR + 7, '0');
    EEPROM_write(WRITE_ADDR + 8, '6');

    // Display 16 bytes on LCD

    lcd_cursor_second_line();
    for (i = 0; i < 16; i++)
      lcd_write_character(EEPROM_read(READ_ADDR + i));

    while (1) ;

    return 0;

}

