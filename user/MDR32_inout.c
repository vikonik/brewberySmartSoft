#include "MDR32_inout.h"
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK

////////////////////////////////////////////////////////////////
///z Инициализация пина в качестве цифрового выхода
////////////////////////////////////////////////////////////////
void PinInit_OUT(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin) 
{	
	/* GPIO Periph clock enable */
	if(PORTx == MDR_PORTA)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	else if (PORTx == MDR_PORTB)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	else if (PORTx == MDR_PORTC)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	else if (PORTx == MDR_PORTD)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	else if (PORTx == MDR_PORTE)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	else if (PORTx == MDR_PORTF)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);
	
	PORT_InitTypeDef PortInit;
	
//	if ((PORT_Pin == LCD_MOSI_PIN) || (PORT_Pin == LCD_CSK_PIN))
//	PortInit.PORT_FUNC = PORT_FUNC_ALTER;    //как порт SPI
//	else 
	PortInit.PORT_FUNC = PORT_FUNC_PORT;    //как порт ввода-вывода

	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	
	PortInit.PORT_OE = PORT_OE_OUT;										//выход	
	PortInit.PORT_Pin = PORT_Pin;											//номер пина
	
	PORT_Init(PORTx, &PortInit);
}
////////////////////////////////////////////////////////////////////////////

/*.........................................................................
Инициализация пина в качестве цифрового входа
.........................................................................*/
void PinInit_IN(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin) 
{
	/* GPIO Periph clock enable */
	if(PORTx == MDR_PORTA)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	else if (PORTx == MDR_PORTB)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	else if (PORTx == MDR_PORTC)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	else if (PORTx == MDR_PORTD)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	else if (PORTx == MDR_PORTE)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	
	PORT_InitTypeDef PortInit;
	
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_DRIVER;								//вроде как "push pull" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	PortInit.PORT_FUNC = PORT_FUNC_PORT;							//как порт ввода-вывода
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	
	PortInit.PORT_OE = PORT_OE_IN;										//вход	
	PortInit.PORT_Pin = PORT_Pin;											//номер пина
	
	PORT_Init(PORTx, &PortInit);
}
/*.......................................................................*/

//////////////////////////////////////////////////////////////////////////
///z Установка выхода (выходов)
//////////////////////////////////////////////////////////////////////////
void PIO_SetBits(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin)
{
	PORT_SetBits(PORTx, PORT_Pin);
}
/////////////////////////////////////////////////////////////////////////
///z Сброс выхода (выходов)
////////////////////////////////////////////////////////////////////////
void PIO_ResetBits(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin)
{
	PORT_ResetBits(PORTx, PORT_Pin);
}
////////////////////////////////////////////////////////////////////////

/*.........................................................................
Чтение цифорового входа
.........................................................................*/
uint8_t PIO_ReadInputDataBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin)
{
	return PORT_ReadInputDataBit(PORTx, PORT_Pin);
}
/*.......................................................................*/


void PIO_Init_Pin(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin, Port_Config config)
{
	/* GPIO Periph clock enable */
	if(PORTx == MDR_PORTA)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	else if (PORTx == MDR_PORTB)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);
	else if (PORTx == MDR_PORTC)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	else if (PORTx == MDR_PORTD)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	else if (PORTx == MDR_PORTE)
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	
	PORT_InitTypeDef PortInit;
	
	if(config == Output_Open_Drain_Up)
	{
		PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
		PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
		PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
		PortInit.PORT_PD = PORT_PD_OPEN;									//open drain
		PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
		PortInit.PORT_FUNC = PORT_FUNC_PORT;							//как порт ввода-вывода
		PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
		PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
		PortInit.PORT_OE = PORT_OE_OUT;										//выход	
		PortInit.PORT_Pin = PORT_Pin;											//номер пина
	
		PORT_Init(PORTx, &PortInit);
	}
	else 
	if(config == Input_Pull_Up)
	{
		PortInit.PORT_PULL_UP = PORT_PULL_UP_ON;					//pull up резистор
		PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
		PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
		PortInit.PORT_PD = PORT_PD_DRIVER;								//push pull
		PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
		PortInit.PORT_FUNC = PORT_FUNC_PORT;							//как порт ввода-вывода
		PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
		PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
		PortInit.PORT_OE = PORT_OE_IN;										//вход	
		PortInit.PORT_Pin = PORT_Pin;											//номер пина
	
		PORT_Init(PORTx, &PortInit);
	}
	else
	if(config == Input_Ext_Interrupt)
	{
		PortInit.PORT_PULL_UP = PORT_PULL_UP_ON;					//pull up резистор
		PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
		PortInit.PORT_PD_SHM = PORT_PD_SHM_ON;						//триггер Шмитта
		PortInit.PORT_PD = PORT_PD_DRIVER;								//push pull
		PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
		
		if( 	(PORTx == MDR_PORTA && PORT_Pin == PORT_Pin_0)
			||	(PORTx == MDR_PORTB && (PORT_Pin == PORT_Pin_10 || PORT_Pin == PORT_Pin_11)) 
		)
			PortInit.PORT_FUNC = PORT_FUNC_ALTER;						//альтернативная функция
		
		if(PORTx == MDR_PORTB && PORT_Pin == PORT_Pin_9)
			PortInit.PORT_FUNC = PORT_FUNC_OVERRID;					//переопределенная функция
		
		PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
		PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
		PortInit.PORT_OE = PORT_OE_IN;										//вход	
		PortInit.PORT_Pin = PORT_Pin;											//номер пина
	
		PORT_Init(PORTx, &PortInit);
		
		// Разрешаем соответствующее прерывание
		NVIC_EnableIRQ(EXT_INT2_IRQn);
	}
}

/*.........................................................................
Изменение состояния выхода на противоположное
.........................................................................*/
void PIO_ToogleBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin)
{
	if(PORT_ReadInputDataBit(PORTx, PORT_Pin))
		PORT_ResetBits(PORTx, PORT_Pin);
	else
		PORT_SetBits(PORTx, PORT_Pin);
}
/*.......................................................................*/

