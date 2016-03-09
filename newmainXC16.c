#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "general.h"
#include "send.h"

#define FCY 16000000UL
#include "libpic30.h"

// FBS
#pragma config BWRP = OFF               // Boot Segment Write Protect (Disabled)
#pragma config BSS = OFF                // Boot segment Protect (No boot program flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Write Protect (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Protect (No Protection)

// FOSCSEL
#pragma config FNOSC = FRCPLL           // Oscillator Select (Fast RC Oscillator with Postscaler and PLL Module (FRCDIV+PLL))
#pragma config SOSCSRC = DIG            // SOSC Source Type (Digital Mode for use with external source)
#pragma config LPRCSEL = HP             // LPRC Oscillator Power and Accuracy (High Power, High Accuracy Mode)
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = CLKO          // CLKO Enable Configuration bit (CLKO output signal enabled)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary Oscillator configured for high-power operation)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Both Clock Switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPS = PS1              // Watchdog Timer Postscale Select bits (1:1)
#pragma config FWPSA = PR128            // WDT Prescaler bit (WDT prescaler ratio of 1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT disabled in hardware; SWDTEN bit disabled)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected(windowed WDT disabled))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware, SBOREN bit disabled)
#pragma config RETCFG = OFF             //  (Retention regulator is not available)
#pragma config PWRTEN = OFF             // Power-up Timer Enable bit (PWRT disabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Use Default SCL1/SDA1 Pins For I2C1)

#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (RA5 input pin disabled, MCLR pin enabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (EMUC/EMUD share PGC2/PGD2)

volatile int isConnected;
volatile int rxState;

void __attribute__((__interrupt__, auto_psv))_ISR _U1RXInterrupt(void) {
    char buffer[8];
    int i = 0;
    
    U1TXREG = isConnected;
    if(U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
    }
    while(U1STAbits.URXDA == 1) {
        if(i<8) {
           buffer[i] = U1RXREG;
        }
        i++;
    }
    switch(rxState) {
        case 0:
            if(buffer[0] == 'O') {
                rxState = 1;
            } else {
                rxState = 0;
            }
            break;
        case 1:
            if(buffer[0] == 'K') {
                rxState = 2;
            } else {
                rxState = 0;
            }
            break;
        case 2:
            if(buffer[0] == '+') {
                rxState = 3;
            } else {
                rxState = 0;
            }
            break;
        case 3:
            if(buffer[0] == 'C') {
                rxState = 4;
            } else if (buffer[0] == 'L') {
                rxState = 7;
            } else {
                rxState = 0;
            }
            break;
        case 4:
            if(buffer[0] == 'O') {
                rxState = 5;
            } else {
                rxState = 0;
            }
            break;
        case 5:
            if(buffer[0] == 'N') {
                rxState = 6;
            } else {
                rxState = 0;
            }
            break;
        case 6:
            if(buffer[0] == 'N') {
                rxState = 0;
                isConnected = 0xFF;
            } else {
                rxState = 0;
            }
            break;
        case 7:
            if(buffer[0] == 'O') {
                rxState = 8;
            } else {
                rxState = 0;
            }
            break;
        case 8:
            if(buffer[0] == 'S') {
                rxState = 9;
            } else {
                rxState = 0;
            }
            break;
        case 9:
            if(buffer[0] == 'T') {
                rxState = 0;
                isConnected = 0xFF;
            } else {
                rxState = 0;
            }
            break;
    }
    //isConnected = 0xFF;
    //Clear flag !
    IFS0bits.U1RXIF = 0;
    
}

void __attribute__((__interrupt__, auto_psv))_ISR _U1TXInterrupt(void){
    ble_send();
    IFS0bits.U1TXIF=0b0;// remise à 0 du flag
    return;
  } // end interrupt

int main(void) {
    isConnected = 0x00;
    CLKDIV = 0x0000;
    ble_config();
    rxState = 0;
    int canSend = 0;
    /// timer/////
        T1CON = 0xA200;
        PR1 = 0x257F;
    /// end timer////
    IFS0bits.T1IF=0; // flag du timer
      
    while(1){
        while(IFS0bits.T1IF == 0){}
        IFS0bits.T1IF=0;
        
        if(isConnected == 0xFF) { 
            isConnected = 0x00;
            if(canSend == 0) {
                __delay_ms(3000);
                canSend = 1;
            } else {
                __delay_ms(1000);
                canSend = 0;
            }
        }
        if(canSend == 1) {
            ble_start();  
        }
    }
            
    return 0;
}



