/*
Здесь описывается назначение выводов микроконтроллера
*/


#ifndef _ALL_DEFENITION_H
#define _ALL_DEFENITION_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT




/************************ Реле **************************/
#define RELAY_PORT  MDR_PORTF
#define MIXER     	PORT_Pin_5
#define HEAT_1     	PORT_Pin_7
#define COOL_1     	PORT_Pin_8


/************************ Дисплей ************************/
#define LCD_RST_PORT                MDR_PORTD
#define LCD_RST_PIN                 PORT_Pin_0

#define LCD_D_C_PORT                MDR_PORTD
#define LCD_D_C_PIN                 PORT_Pin_1

#define LCD_MOSI_PORT               MDR_PORTD
#define LCD_MOSI_PIN                PORT_Pin_2

#define LCD_CSK_PORT                MDR_PORTD
#define LCD_CSK_PIN                 PORT_Pin_4

#define LCD_CS_PORT                 MDR_PORTD
#define LCD_CS_PIN                  PORT_Pin_5

#define MCU_LCD_LED_PORT            MDR_PORTF
#define MCU_LCD_LED_PIN             PORT_Pin_3

//#define LCD_RST_PORT                MDR_PORTF
//#define LCD_RST_PIN                 PORT_Pin_4

/****************** Звук **********************/

#define PORT_BUZZER    MDR_PORTB
#define PIN_BUZZER     PORT_Pin_10


/****************** Кнопки **********************/
#define PORT_MPR_121				MDR_PORTE
#define PIN_MPR_121_INT			PORT_Pin_2
#define PORT_I2C_SOFT				MDR_PORTE								//Софтварный I2C для опроса кнопок микросхемы MPR121
#define PIN_I2C_SOFT_SCL		PORT_Pin_1
#define PIN_I2C_SOFT_SDA		PORT_Pin_0

#define BUTTON_PRESS_DELAY 2000 //Долгое нажате кнопки

#define BUTTON_UP	0x0004
#define BUTTON_DN 0x0008
//								0x0010
#define BUTTON_OK 0x0020

#define S1 0x0800
#define S2 0x0400
#define S3 0x0200
#define S4 0x0100
#define S5 0x0080
#define S6 0x0040

// ****************** Макросы для дробления больших чисел **********************
#define HI3(Int)        (uint8_t)(Int>>24)  // старший байт 32-ного числа
#define HI2(Int)        (uint8_t)(Int>>16)  // старший байт 24-ного числа
#define HI(Int)         (uint8_t)(Int>>8)   // старший байт 16-ного числа
#define LOW(Int)        (uint8_t)(Int&0xFF) // младший байт любого числа
// ****************** Макросы для объединения чисел ****************************
#define BT16(hi,low)    ((hi << 8)|low)          // два char в int

//программный I2C
#pragma pack(push, 1)
typedef struct xI2C 																//Структура софтварного I2C
{
  MDR_PORT_TypeDef*	port;           								// Порт интерфейса
  PORT_Pin_TypeDef	pin_sda;        								// Пин SDA
  PORT_Pin_TypeDef	pin_scl;        								// Пин SCL
  uint8_t      		tim;            								// Задержка в мкс
} xI2C;
#pragma pack(pop)



/********** Светодиоды **********************/
#define PORT_HC595_SPI		MDR_PORTB
#define PIN_HC595_CSK			PORT_Pin_14	
#define PIN_HC595_MOSI		PORT_Pin_13

#define PORT_HC595_CS			MDR_PORTB
#define PIN_HC595_CS			PORT_Pin_15	

/***************** UART *******************/
#define PORT_UART     			MDR_PORTC
#define PIN_UART_RX   			PORT_Pin_4
#define PIN_UART_TX   			PORT_Pin_3
#define ONEWIRE_UART				MDR_UART1
/***************** WiFi ***********************/
//overaid
#define PORT_WiFI     			MDR_PORTC
#define PIN_WiFI_RX   			PORT_Pin_14
#define PIN_WiFI_TX   			PORT_Pin_13
#define WiFI_UART						MDR_UART2
/***************** Flash **********************/
#define SPI_FLASH						MDR_SSP1
#define PORT_FLASH     			MDR_PORTC
#define PIN_FLASH_MOSI			PORT_Pin_5
#define PIN_FLASH_MISO			PORT_Pin_6
#define PIN_FLASH_SCK				PORT_Pin_7
#define PIN_FLASH_CS				PORT_Pin_8


typedef enum{
    COOK_TOP_1,
    COOK_TOP_2,
    COOK_TOP_3,
    COOK_TOP_4,    
    COOK_TOP_NUM
}Cooktop_num;

typedef enum{
	CONNECT_NO,
	CONNECT_WiFi,
	CONNECT_BT
}Connect_t;

/**************************** Структура состояния устройства *****************************/
typedef enum{
	MESH,
	FILTRATION,
	NAGREV,
	COLD,
	FERMENTATION
}ManualControl_e;

typedef struct{
	float targetTemperature;//Целевая темпераатура
	uint16_t targetTimer_h;//Таймер часы
	uint8_t targetTimer_m;	//Таймер Минуты
	uint8_t targetTimer_s;
	uint8_t state;//вкл/откл
}ShablonlControl_t;
extern ShablonlControl_t manualControl[];

typedef struct{
	char id[39];													// ID устройства, "00000000-0000-0000-0000-000000000000" + символ конца строки
	uint16_t woshingTime;//Оставшееся время мойки, ополаскивания. дезинфекции
	
	float temperatureCurrent;
	uint8_t temperatureTurget;
	ShablonlControl_t manualControlCurrentData;//Текущее состояние ручного управления, измеренная температура и оставшееся время
  uint8_t flagRegimOn;
	uint8_t pidEnable;
	uint8_t btStatus;//Состояние BT вкл/откл
	uint8_t wifiStatus;
	uint8_t isMuted;//0-звук есть 1 - звука нет
	Connect_t isConnected;//К чему подключено устройство
	uint8_t isLocked;// 1-заблокировано
	uint8_t isWoshing; //Режим мойки
}DeviceStatus_t;
extern DeviceStatus_t deviceStatus;
#endif

/*

{"id":"12000000-0000-0000-0000-000000000000","washing_time":3600,"temperature_current":22.7,
"heat_temperature":24,"flag_regim_on":1,"pid_enable":1,"bt_status":1,"wifi_status":1,"mute":0}
*/



