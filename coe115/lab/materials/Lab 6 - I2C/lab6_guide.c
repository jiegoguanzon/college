#include "xc.h"

_CONFIG1 (FWDTEN_OFF & JTAGEN_OFF)
_CONFIG2 (POSCMOD_NONE & OSCIOFNC_ON & FCKSM_CSCMD & FNOSC_FRCPLL & PLL96MHZ_OFF & PLLDIV_NODIV)
_CONFIG3 (SOSCSEL_IO)

// Initialize I2C module
void I2C_init(void)
{	
// Set values for the ff:
// I2C1BRG
// I2C1CON
// IEC1	
// Enable I2C after configuration
}

// Send start bit sequence
void I2C_start(void)
{
// Set Start Condition Enabled Bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
}

// Send repeated start bit sequence
void I2C_restart(void)
{
// Set Repeated Start Condition Enabled bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
}

// Send stop bit sequence
void I2C_stop(void)
{
// Set Stop Condition Enabled bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
}

// Send ACK bit sequence
void I2C_ack(void)
{
// Set Acknowledge Data bit
// Set Acknowledge Sequence Enabled bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
}

// Send NACK bit sequence
void I2C_nack(void)
{
// Set Acknowledge Data bit
// Set Acknowledge Sequence Enabled bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
}

// Transfer one byte sequence
bit I2C_write(unsigned char Byte)
{
// Write byte to transmit buffer register
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
// Return Acknowledge bit received
}

// Receive one byte sequence
unsigned I2C_read(void)
{
// Set Receive Enable bit
// Wait for operation to finish by checking interrupt flag
// Clear interrupt flag
// Return receive buffer contents
}

// EEPROM write sequence
void EEPROM_write(unsigned int waddr, unsigned char wdata)
{
// Start I2C communication
I2C_start();

// Send I2C EEPROM slave address with write command, wait if device is not available
while(I2C_write(EEPROM_slave_address + 0)){
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
void EEPROM_read(unsigned int raddr)
{
  unsigned char Byte = 0;

// Start I2C communication
I2C_start();

// Send I2C EEPROM slave address with write command, wait if device is not available
while(I2C_write(EEPROM_slave_address + 0)){
  I2C_restart();
}

// Send address, high byte then low byte
I2C_write(raddr>>8);
I2C_write(raddr);

// Send repeated start
I2C_restart();

// Send I2C EEPROM slave address with read command
I2C_write(EEPROM_slave_address + 1);

// Store data from buffer
Byte = I2C_read();

// Send NACK
I2C_nack();

// Stop I2C communication
I2C_stop();

return Byte;

}

void main(void)
{
// Initialize I2C peripheral
// Initialize LCD
// Write student number to WRITE_ADDR
// Read 16 bytes from READ_ADDR
// Display 16 bytes on LCD

while(1){}

}
