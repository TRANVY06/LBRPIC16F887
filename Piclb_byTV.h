

/* 
 * File:   LIB
 * Author: NGOANKLEIN
 * Comments: Enjoy it!
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
//#ifndef _PIC_MAIN_H
#ifndef _PIC_MAIN_H
#define _PIC_MAIN_H

#endif
#include <stdbool.h>
// #include <xc.h> // include processor files - each processor file is guarded.  
#define _XTAL_FREQ 20000000
#pragma config FOSC = HS        // High-Speed Oscillator (d�ng th?ch anh ngo�i > 4MHz, ? ?�y 24MHz)
#pragma config WDTE = OFF       // Watchdog Timer disabled
#pragma config PWRTE = ON       // Power-up Timer enabled
#pragma config BOREN = ON       // Brown-out Reset enabled
#pragma config LVP = OFF        // Low-Voltage Programming enabled
#pragma config CPD = ON        // Data EEPROM Memory Code Protection off
#pragma config WRT = OFF        // Flash Program Memory Write Protection off
#pragma config CP = ON
#define HIGH_1 (0X17)
#define LOW_1 (0xFF)

/*if st==8 is analogRead_8bits
  if st==10 is analogRead_10bits*/
void analogRead_init(unsigned char st); // st have only 8 and 10 
unsigned int analogRead_8bits(unsigned char pin); //Return 8 bits result from 0 to 255 
unsigned int analogRead_10bits(unsigned char pin); //Return 10 bits result from 0 to 1023
void analogWrite_8bits(unsigned char cp1, unsigned char cp2); //CCPR1L=cp1;CCPR2L=cp2 
void analogWrite_init(unsigned char frequency); // important; frequency: HIGH/ LOW; PW: pulse width
void I2C_Master_Wait(void); // Wait for I2C bus to be idle
void I2C_Master_Init(const unsigned long Br);  //Br is the baud rate
void I2C_Master_Start(); // Send I2C start condition
void I2C_Master_Stop();  // Send I2C stop condition
void I2C_Master_Write(unsigned data); // Ghi dữ liệu
unsigned short I2C_Master_Read(unsigned short a); // a=1: return ACK; a=0: return NACK
void I2C_Slave_Init(short address); // address: 7 bit address


