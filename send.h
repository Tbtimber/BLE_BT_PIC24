

#ifndef _UART_H
#define	_UART_H

#include "general.h"

UINT8_T shiftLeft(unsigned short data); // shift les 6 premiers bits en sortie du converteur (XXXXXX000000)
UINT8_T shiftRight(unsigned short data); // shift les 6 derniers bits en sortie du converteur (00000XXXXXX)
void ble_send();
void vle_frame_formating();
void ble_config();
void ble_start();

void UARTInit(void);           // init UART with specified baud rate
UINT8_T UARTReadByte(void);                 // read a byte from UART
void UARTWriteByte(UINT8_T data);           // write a byte to UART
void UARTBasicCommand();
void UARTConfigBaudRate215400();
void UARTWriteChar(char data);
void UARTWakeUp();
void resetBTModule();
void UARTInit230400(void);
void UARTInit9600(void);
void UARTSetBTName();
void UARTSleep();
void initBTModule();
void UARTSetADTypeIOSVal();
#endif	/* _UART_H */

