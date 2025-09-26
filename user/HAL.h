#ifndef _HAL_H
#define _HAL_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

//#define BUTTON_ON 0x0800
//#define BUTTON_TEMP 0x0400
//#define BUTTON_CHANGE_FUNCTION 0x0400
//#define BUTTON_FERMENT 0x0200
//#define BUTTON_DOWN 0x0200
//#define BUTTON_FILTRATION 0x0080
//#define BUTTON_UP 0x0080
//#define BUTTON_SET_TIMER 0x0040
//#define BUTTON_RESET_DELAY_START 0x0040
//#define BUTTON_SET_START_DELAY 0x0020
//#define BUTTON_SET_TIME 0x0020

#define OFF		0		//выключено
#define ON 		1		//включено
#define DELAY	2		//отложенный старт



typedef enum{
	DEVICE_OFF,
	DEVICE_On,
	DEVICE_LOC
}DeviceState_t;



//typedef struct{
//	uint8_t state;
//	uint8_t temperatureTarget;
//	uint8_t temperatureCurrent;	
//	uint8_t heatStatus;//Состояние нагрева
//	uint8_t fermrntationStatus;
//	uint8_t filtrationStatus;
//	uint8_t isDelayStart;
//	uint16_t delayStart[4];//время отложенного старта
//	uint8_t isFermrntationDelay;//Выбран ли отложенный старт
//	uint8_t isFiltrationDelay;	//Выбран ли отложенный старт
//	uint16_t 	timer;
//	uint8_t 	isTimerOn;
//	uint8_t delayStartCnt; //Номер задержки
//}DeviceStatus_t;
//extern DeviceStatus_t deviceStatus;

//void (*mainProcess)(void);
void functionNull(void);
	
void initDevice(void);

void expressTest(void);
void buttonInit(void);
void fermentationFunction(void);
void filtrationFunction(void);
void allFunctionStop(void);
void printFermentationLabel(void);
void printFiltrationLabel(void);
void printStartDelayLabel(uint64_t blincStateCnt);
void processTemperatureControl(float targetTemp, uint32_t pumpOnTime, uint32_t pumpOffTime);
void beep(uint8_t num);
void displayPinTest(void);
#endif
