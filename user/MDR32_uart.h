#ifndef _MDR32_UART_H
#define _MDR32_UART_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

#include "stdbool.h"
//#include "hardlayer.h"

#define UART_BUFF_SIZE					512

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct
//{
//	MDR_UART_TypeDef    * uart;
//} s_uart;
	
#pragma pack(push, 1)
typedef struct {  																//Структура UART
	//UART_InitTypeDef *	uart;
	MDR_UART_TypeDef *	uart;
	MDR_PORT_TypeDef * 	port;												//Порт ввода-вывода
	uint32_t 						rx;													//Пин порта ввода-вывода, RX
	uint32_t 						tx;													//Пин порта ввода-вывода, TX
	uint32_t						br;													//Скорость подключения, бод
	uint8_t							buf_rx[UART_BUFF_SIZE];			//Буфер приема
	uint16_t						rx_counter;									//Счетчик принятых байт с начала пакета
	uint8_t							receiveFinished;						//Флаг окончания приема пакета данных (выставляется по таймеру Modbus)
	uint8_t							receivedDataOfValidLength;	//Флаг устанавливается, когда количество байт в RX буфере >= необходимому размеру пакета
	uint8_t							buf_tx[UART_BUFF_SIZE];			//Буфер отправления
	uint16_t						tx_counter;									//Счетчик отправленных байт
	uint16_t						bytesToSend;								//Сколько байтов нужно отправить, прежде чем выставится флаг sendFinished
	uint8_t 						sendFinished;								//Флаг окончания передачи пакета данных
} s_uart;								
#pragma pack(pop)

	
s_uart* OneWire_InitUart(void);
bool HAL_UART_Transmit(s_uart * uartx, uint8_t *pData, uint16_t size, uint32_t timeout);
bool HAL_UART_Receive(s_uart * uartx, uint8_t *pData, uint16_t size, uint32_t timeout);
void SetBaudrate(s_uart* uartx, uint32_t baudrate);
void InitDebugUart(s_uart * uartx);

void UARTInit(s_uart *uport);
uint8_t IsUartDataReceived(s_uart *uport);
void UartNextReady(s_uart * uport);
void UART_PauseNill(MDR_TIMER_TypeDef* TIMERx); 
void UartSendByte(s_uart *uport, uint8_t data);
void UartClearRxBuffer(s_uart *uport);
void UartSendData(s_uart * uport, uint8_t * data, uint16_t size);
	

#ifdef __cplusplus
}
#endif	

#endif		/* _MDR32_UART_H */
