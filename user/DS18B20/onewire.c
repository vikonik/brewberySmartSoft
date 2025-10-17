/* Includes ------------------------------------------------------------------*/

#include "onewire.h"


/* Declarations and definitions ----------------------------------------------*/

/* Functions -----------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
uint8_t OneWire_ProcessBit(s_uart *uartx, uint8_t bit)
{
  uint8_t txData = 0xFF;
  uint8_t rxData = 0x00;

  if (bit == 0)
  {
    txData = 0x00;
  }

  HAL_UART_Transmit(uartx, &txData, 1, ONEWIRE_UART_TIMEOUT);
  HAL_UART_Receive(uartx, &rxData, 1, ONEWIRE_UART_TIMEOUT);

  return rxData;
}


/*----------------------------------------------------------------------------*/
uint8_t OneWire_ProcessByte(s_uart *uartx, uint8_t byte)
{
  uint8_t rxByte = 0x00;

  for (uint8_t i = 0; i < ONEWIRE_BITS_NUM; i++)
  {
    uint8_t txBit = (byte >> i) & 0x01;
    uint8_t rxBit = 0;

    uint8_t tempRxData = OneWire_ProcessBit(uartx, txBit);

    if (tempRxData == 0xFF)
    {
      rxBit = 1;
    }

    rxByte |= (rxBit << i);
  }

  return rxByte;
}


/*----------------------------------------------------------------------------*/
ONEWIRE_Status OneWire_Reset(s_uart *uartx)
{
  OneWire_ProcessByte(uartx, 0x43);

  ONEWIRE_Status status = ONEWIRE_OK;
  uint8_t txByte = ONEWIRE_RESET_BYTE;
  uint8_t rxByte = 0x00;

  SetBaudrate(uartx, ONEWIRE_RESET_BAUDRATE);

  HAL_UART_Transmit(uartx, &txByte, 1, ONEWIRE_UART_TIMEOUT);
  HAL_UART_Receive(uartx, &rxByte, 1, ONEWIRE_UART_TIMEOUT);

  SetBaudrate(uartx, ONEWIRE_BAUDRATE);

  if (rxByte == txByte)
  {
    status = ONEWIRE_ERROR;
  }

  return status;
}


/*----------------------------------------------------------------------------*/
s_uart* OneWire_Init(void)
{
	return OneWire_InitUart();
}


/*----------------------------------------------------------------------------*/

