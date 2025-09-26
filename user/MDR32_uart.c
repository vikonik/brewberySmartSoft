#include "MDR32_uart.h"
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_uart.h"             // Milandr::Drivers:UART
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT

//#include "MDR32_rcc.h"
//#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "string.h"
#include "alldefenition.h"
#include "delay.h"
static UART_InitTypeDef onewireUART_InitTypeDef;
//static UART_InitTypeDef debugUART_InitTypeDef;
s_uart onewire;

/*----------------------------------------------------------------------------*/
void SetBaudrate(s_uart* uartx, uint32_t baudrate)
{
	UART_Cmd(uartx->uart, DISABLE);

	if(uartx->uart == MDR_UART1)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	if(uartx->uart == MDR_UART2)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
	
  onewireUART_InitTypeDef.UART_BaudRate = baudrate;

  UART_Init(uartx->uart, &onewireUART_InitTypeDef);
  UART_BRGInit(uartx->uart,UART_HCLKdiv1);
  UART_Cmd(uartx->uart, ENABLE);
}


/*----------------------------------------------------------------------------*/
s_uart* OneWire_InitUart(void)
{
	onewire.uart = MDR_UART1;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	//настроить порты, UART_TXD1 - PC3, UART_RXD1 - PC4; UART_TXD2 - PD13, UART_RXD2 - PD14
	
	if(onewire.uart == MDR_UART1)
	{
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	}
	if(onewire.uart == MDR_UART2)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	
	PORT_InitTypeDef PortInit;
	
	//RX
	PortInit.PORT_OE = PORT_OE_IN;										//вход	
	PortInit.PORT_Pin = PIN_UART_RX;									//номер пина
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_OPEN;									//вроде как "open drain" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	PortInit.PORT_FUNC = PORT_FUNC_MAIN;							//как основная функция
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	PORT_Init(PORT_UART, &PortInit);
	
	//TX
	PortInit.PORT_OE = PORT_OE_OUT;										//выход	
	PortInit.PORT_Pin = PIN_UART_TX;									//номер пина
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_DRIVER;								//вроде как "push pull" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	PortInit.PORT_FUNC = PORT_FUNC_MAIN;							//как основная функция
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	PORT_Init(PORT_UART, &PortInit);
	
	UART_DeInit(onewire.uart);
	
  UART_StructInit(&onewireUART_InitTypeDef);
	if(onewire.uart == MDR_UART1)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	if(onewire.uart == MDR_UART2)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

  onewireUART_InitTypeDef.UART_BaudRate = 9600;
  onewireUART_InitTypeDef.UART_WordLength = UART_WordLength8b;
  onewireUART_InitTypeDef.UART_StopBits = UART_StopBits1;
  onewireUART_InitTypeDef.UART_Parity = UART_Parity_No;
  onewireUART_InitTypeDef.UART_FIFOMode = UART_FIFO_OFF;
  onewireUART_InitTypeDef.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE|UART_HardwareFlowControl_TXE;

  UART_Init(onewire.uart, &onewireUART_InitTypeDef);
  UART_BRGInit(onewire.uart,UART_HCLKdiv1);
  UART_Cmd(onewire.uart, ENABLE);	
	
	return &onewire;
}


/**/
//HAL_UART_Transmit(huart, &txData, 1, ONEWIRE_UART_TIMEOUT);
//HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
bool HAL_UART_Transmit(s_uart * uartx, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	int i;
	uint64_t start_time = millis();
	
	for(i=0; i<size; i++)
	{
		/* Check TXFE flag*/
		//while (UART_GetFlagStatus(uartx->uart, UART_FLAG_BUSY))			//UART_FLAG_TXFE!= SET UART_FLAG_BUSY
		while (UART_GetFlagStatus(uartx->uart, UART_FLAG_TXFF))
		{
			if(millis()-start_time > timeout)
				break;
		};	

		if(millis()-start_time > timeout)
			break;
		
		UART_SendData(uartx->uart, pData[i]);
		
		//while (UART_GetFlagStatus(uartx->uart, UART_FLAG_BUSY))	
		while (UART_GetFlagStatus(uartx->uart, UART_FLAG_TXFF))	
		{
			if(millis()-start_time > timeout)
				break;
		};
	}
	
	if(i == size)
		return true;
	else
		return false;
}


//HAL_UART_Receive(uartx, &rxByte, 1, ONEWIRE_UART_TIMEOUT);
//HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
bool HAL_UART_Receive(s_uart * uartx, uint8_t *pData, uint16_t size, uint32_t timeout)
{
	int i;
	uint64_t start_time = millis();
	
	for(i=0; i<size; i++)
	{		
		while (UART_GetFlagStatus(uartx->uart, UART_FLAG_BUSY))	
		{
			if(millis()-start_time > timeout)
				break;
		};
		
		if(millis()-start_time > timeout)
			break;
		
		pData[i] = UART_ReceiveData(uartx->uart);
	}
	
	if(i==size)
		return true;
	else
		return false;
}


void InitDebugUart(s_uart * uartx)
{
//	uartx->uart = DEBUG_UART;
//	//RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
//	
//	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
//	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
//	
//	PORT_InitTypeDef PortInit;
//	
//	//RX
//	PortInit.PORT_OE = PORT_OE_IN;										//вход	
//	PortInit.PORT_Pin = PIN_DEBUG_UART_RX;						//номер пина
//	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
//	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
//	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
//	PortInit.PORT_PD = PORT_PD_OPEN;									//вроде как "open drain" режим
//	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
//	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;						//как переопределенная функция
//	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
//	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
//	PORT_Init(PORT_DEBUG_UART, &PortInit);
//	
//	//TX
//	PortInit.PORT_OE = PORT_OE_OUT;										//выход	
//	PortInit.PORT_Pin = PIN_DEBUG_UART_TX;						//номер пина
//	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
//	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
//	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
//	PortInit.PORT_PD = PORT_PD_DRIVER;								//вроде как "push pull" режим
//	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
//	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;						//как переопределенная функция
//	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
//	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
//	PORT_Init(PORT_DEBUG_UART, &PortInit);
//	
//	UART_DeInit(uartx->uart);
//	
//  UART_StructInit(&debugUART_InitTypeDef);
//	if(uartx->uart == MDR_UART1)
//		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
//	if(uartx->uart == MDR_UART2)
//		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

//  //debugUART_InitTypeDef.UART_BaudRate = 115200;
//	debugUART_InitTypeDef.UART_BaudRate = 460800;
//	//debugUART_InitTypeDef.UART_BaudRate = 921600;
//  debugUART_InitTypeDef.UART_WordLength = UART_WordLength8b;
//  debugUART_InitTypeDef.UART_StopBits = UART_StopBits1;
//  debugUART_InitTypeDef.UART_Parity = UART_Parity_No;
//  debugUART_InitTypeDef.UART_FIFOMode = UART_FIFO_OFF;
//	//debugUART_InitTypeDef.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
//  debugUART_InitTypeDef.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE|UART_HardwareFlowControl_TXE;

//  UART_Init(uartx->uart, &debugUART_InitTypeDef);
//  UART_BRGInit(uartx->uart,UART_HCLKdiv1);
//  UART_Cmd(uartx->uart, ENABLE);	
}

uint8_t IsUartDataReceived(s_uart *uport) 
{
	return (uport->receivedDataOfValidLength && uport->receiveFinished); 
}

void UartNextReady(s_uart * uport) 
{
	uport->receiveFinished = false;
	uport->receivedDataOfValidLength = false;
}

void UARTInit(s_uart *uport) 
{  
	if(uport->uart == MDR_UART1)
	{
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);
	}
	if(uport->uart == MDR_UART2)
	{
		RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
	}
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	PORT_InitTypeDef PortInit;
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_DRIVER;								//вроде как "push pull" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	//PortInit.PORT_FUNC = PORT_FUNC_ALTER;							//альтернативная функция
	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;							//переопределенная функция
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	
	/* Configure USART Tx as alternate function push-pull */
	PortInit.PORT_OE = PORT_OE_OUT;										//выход
	PortInit.PORT_Pin = uport->tx;											//номер пина
  if(uport->uart == MDR_UART1)
	{
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
		PORT_Init(MDR_PORTC, &PortInit);
	}
	if(uport->uart == MDR_UART2)
	{
		if(PortInit.PORT_FUNC == PORT_FUNC_MAIN) 
		{
			RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
			PORT_Init(MDR_PORTD, &PortInit);
		}
		else if(PortInit.PORT_FUNC == PORT_FUNC_OVERRID)
		{
			RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
			PORT_Init(MDR_PORTC, &PortInit);
		}
	}
				
  /* Configure USART Rx as input floating */
	PortInit.PORT_OE = PORT_OE_IN;										//вход	
	PortInit.PORT_Pin = uport->rx;											//номер пина
  if(uport->uart == MDR_UART1)
	{
		PORT_Init(MDR_PORTC, &PortInit);
	}
	if(uport->uart == MDR_UART2)
	{
		if(PortInit.PORT_FUNC == PORT_FUNC_MAIN) 
		{
			PORT_Init(MDR_PORTD, &PortInit);
		}
		else if(PortInit.PORT_FUNC == PORT_FUNC_OVERRID)
		{
			PORT_Init(MDR_PORTC, &PortInit);
		}
	}
	
	UART_DeInit(uport->uart);

	/* Set the HCLK division factor = 1*/
  UART_BRGInit(uport->uart, UART_HCLKdiv1);
	
	/* Configure USART */
	UART_InitTypeDef UART_InitStructure;
	UART_InitStructure.UART_BaudRate            = uport->br;
  UART_InitStructure.UART_WordLength          = UART_WordLength8b;
  UART_InitStructure.UART_StopBits            = UART_StopBits1;
  UART_InitStructure.UART_Parity              = UART_Parity_No;
  UART_InitStructure.UART_FIFOMode            = UART_FIFO_OFF;
  //UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;		//RXE и TXE просто отключаются в SPL без flow control ??!
	UART_InitStructure.UART_HardwareFlowControl = (UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE);
	UART_Init(uport->uart, &UART_InitStructure);
  	 
  /* Enable USART Receive and Transmit interrupts */
  UART_ITConfig(uport->uart, UART_IT_RX, ENABLE);
	UART_ITConfig(uport->uart, UART_IT_TX, ENABLE);

  /* Enable the USART */
  UART_Cmd(uport->uart, ENABLE);	
	
  /* Enable the USART Interrupt */	
	if(uport->uart == MDR_UART1)
	{
		NVIC_EnableIRQ(UART1_IRQn);
	}
	if(uport->uart == MDR_UART2)
	{
		NVIC_EnableIRQ(UART2_IRQn);
	}
}

void UART_PauseNill(MDR_TIMER_TypeDef* TIMERx) 
{
//	TIMER_SetCounter(TIMERx, 0);
//	
//	if (TIMERx == MDR_TIMER1) 
//	{
//		TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
//		NVIC_EnableIRQ(TIMER1_IRQn);		
//	}
//	if (TIMERx == MDR_TIMER2)
//	{
//		TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
//		NVIC_EnableIRQ(TIMER2_IRQn);
//	}
//	if (TIMERx == MDR_TIMER3)
//	{
//		TIMER_ClearFlag(MDR_TIMER3, TIMER_STATUS_CNT_ARR);
//		NVIC_EnableIRQ(TIMER3_IRQn);
//	}
//	if (TIMERx == MDR_TIMER4)
//	{
//		TIMER_ClearFlag(MDR_TIMER4, TIMER_STATUS_CNT_ARR);
//		NVIC_EnableIRQ(TIMER4_IRQn);
//	}
}

void UartSendByte(s_uart *uport, uint8_t data)
{
	UART_SendData(uport->uart, data);
}

void UartClearRxBuffer(s_uart *uport)
{
	memset(uport->buf_rx, 0x00, UART_BUFF_SIZE);
	uport->rx_counter = 0;
}

void UartSendData(s_uart * uport, uint8_t * data, uint16_t size)
{
	while(uport->sendFinished == false)			//Ждём окончания предыдущей передачи, перед тем как перезаписать буфер новыми данными
	{}		
	memcpy(uport->buf_tx, data, size);
	uport->sendFinished = false;
	uport->bytesToSend = size;
	UartSendByte(uport, uport->buf_tx[0]);
}

