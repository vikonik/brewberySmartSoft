/**
  ******************************************************************************
  * @file           : onewire.h
  * @brief          : 1-Wire driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef ONEWIRE_H
#define ONEWIRE_H



/* Includes ------------------------------------------------------------------*/

#include "MDR32FxQI_uart.h"             // Milandr::Drivers:UART
#include <stdint.h>
#include "MDR32_uart.h"



/* Declarations and definitions ----------------------------------------------*/

#define ONEWIRE_BAUDRATE                                              115200
#define ONEWIRE_RESET_BAUDRATE                                        9600

#define ONEWIRE_RESET_BYTE                                            0xF0
#define ONEWIRE_UART_TIMEOUT                                          10
#define ONEWIRE_BITS_NUM                                              8



typedef enum
{
  ONEWIRE_OK              = 0x00,
  ONEWIRE_ERROR           = 0x01,
} ONEWIRE_Status;



/* Functions -----------------------------------------------------------------*/

ONEWIRE_Status OneWire_Reset(s_uart *uartx);
uint8_t OneWire_ProcessByte(s_uart *uartx, uint8_t byte);
uint8_t OneWire_ProcessBit(s_uart *uartx, uint8_t bit);
//void OneWire_Init(s_uart *uartx);
s_uart* OneWire_Init(void);



#endif // #ifndef ONEWIRE_H
