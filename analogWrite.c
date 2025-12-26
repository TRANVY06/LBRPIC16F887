/*
 * File:   analogWrite.c
 * Author: DELL
 *
 * Created on December 11, 2025, 1:05 PM
 */


#include <xc.h>
#include "Piclb_byTV.h"

void analogWrite_init(unsigned char frequency) {
    PR2 = frequency;

    CCP2CON = 0x0C;
    CCP1CON = 0x0C;

    TRISC1 = 0;
    TRISC2 = 0;

    CCPR1L = 0;
    CCPR2L = 0;

    TMR2 = 0X00;

    T2CON = 0b00000100; // TMR2ON = 1, prescale 1:1
    /*bit7 bit6 bit5 bit4   bit3    bit2    bit1    bit0
        0    0    0    0  TOUTPS3 TOUTPS2 TOUTPS1 TOUTPS0
                                   TMR2ON  T2CKPS1  T2CKPS0
     */

}

void analogWrite_8bits(unsigned char cp1, unsigned char cp2) {
    CCPR1L = cp1;
    CCPR2L = cp2;
}
