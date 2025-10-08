#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_ssp.h"              // Milandr::Drivers:SSP
#include "MDR32FxQI_i2c.h"
#include "MDR32FxQI_timer.h"            // Milandr::Drivers:TIMER
#include "MDR32FxQI_uart.h"             // Milandr::Drivers:UART


#include "HAL.h"
#include "allDefenition.h"
#include "delay.h"
#include <stdio.h>
#include "I2C_soft.h"
#include "MPR121.h"
#include "MDR32_inout.h"
#include "hc595.h"

#include "functional.h"
#include "HAL.h"
#include "pid.h"
#include "mainPage.h"
#include "menu.h"
#include "menuManualControl.h"
#include "ds18b20.h"
#include "tempSensor.h"
#include "menuSetting.h"
#include "slider.h"
#include "json.h"

extern void (*mainProcess)(void);

ManualControl_t manualControl[4] = {
    {
        .targetTemperature = 0.0,
        .targetTimer_h = 0,
        .targetTimer_m = 10,
        .state = 0
    },
    {
        .targetTemperature = 0.0,
        .targetTimer_h = 0,
        .targetTimer_m = 0,
        .state = 0
    },
    {
        .targetTemperature = 0.0,
        .targetTimer_h = 0,
        .targetTimer_m = 0,
        .state = 0
    },
    {
        .targetTemperature = 0.0,
        .targetTimer_h = 0,
        .targetTimer_m = 0,
        .state = 0
    }
};

//Надо менять назначение кнопок при настройке параметоров
void (*buttonNavigationFunction)(void);
void menuNavigationFunction(void);
void menuChangeDataFunction(void);
uint16_t allButtonsRAW = 0;

/********** Для кнопки *********/
uint64_t buttonPressStartTime = 0;
uint64_t lastIncrementTime = 0;
uint8_t buttonUpWasPressed  = 0;
uint8_t buttonDownWasPressed   = 0;
const unsigned long INITIAL_DELAY = 2000;    // задержка перед ускорением (мс)
const unsigned long ACCELERATED_DELAY = 100; // интервал при ускорении (мс)
const unsigned long DEBOUNCE_DELAY = 50;     // защита от дребезга (мс)
/*******************************/


volatile uint16_t cnt = 0;
/*********************/
void initTestI2CPin(void);
void initCLK(void);
void initSPI(void);
void initResetPin(void);
void initLCDLed(void);
void initI2C(void);
void cooktopInit(void);
void buttonRead(void);
void setCoockTopDeselect(void);
void initRelay(void);
void initLcdLed(void);
void initBuzzer(void);
void initBuzerTimer(void);    
void testDevice(void);
void initMainTimer(void);
/********************/
uint8_t buzerEn = 0;
uint64_t printIndicateCnt = 0;
uint64_t delayStartSec = 0;
uint64_t blincStateCnt = 0;


uint64_t delayTimerSec = 0;
char buf[32];
uint32_t keyBuf[255];
uint8_t keyBufCnt = 0;


extern uint8_t displayArray[];
extern uint8_t digit[];
uint8_t startDelayNum = 0;
uint8_t blincTimetCny = 0;

uint64_t readTemperatureTimer = 0;
//extern DS18B20 ds18b20; 

uint64_t reinitButton = 0;//Таймер переинициализации кнопок

uint64_t timeButtonLock = 0;//Блокировка кнопок
uint8_t triggerButtonLock = 0;
uint64_t timeNow = 0;
uint64_t timeButtonPres = 0;

uint8_t stepUnLOC = 0;


/***********************************/

int main(void){
	mainProcess = functionNull;//Ставим затычку.
	pid_init(&pid, 2.0, 0.1, 0.5, 22.0);
		initDevice();
//while(1){
//UART_SendData(MDR_UART1, 0x55);
//	delay_ms(100);
//}
 // expressTest();
	initMenuMainPage();
	buttonNavigationFunction = menuNavigationFunction;//Установили функции кнопок
while(1){
	printMainPage(deviceStatus.isLocked,deviceStatus.isConnected);
		// Обработка входящих JSON сообщений
//	process_incoming_json(&uart_parser, &deviceStatus);
	
//	if(millis() - reinitButton > (1*30*1000)){
//		reinitButton = millis();
//		MPR121initRresult = mpr121_init(0x5A);
//	}
		if(PIO_ReadInputDataBit(PORT_MPR_121, PIN_MPR_121_INT) == 0)	//Когда контроллер сенсорных кнопок замечает изменение состояния кнопок, он выставляет линию в 0, пока данные не вычитаны
		{
			//_delay_ms(10);
			allButtonsRAW = mpr121_get_touch(MPR121_ADDRESS_BUTTONS);		
			
			if(allButtonsRAW != 0){	
				if((allButtonsRAW & S1)!= S1 && (allButtonsRAW & S2)!= S2 &&(allButtonsRAW & S3)!= S3 &&(allButtonsRAW & S4)!= S4 &&(allButtonsRAW & S5)!= S5 && (allButtonsRAW & S6)!= S6 )
					beep(1);
			}
		}	
		
		if(!!(allButtonsRAW & S1)){
			if(millis() - timeButtonLock > 3000){
				timeButtonLock = millis() ;
				if(!triggerButtonLock){
					triggerButtonLock = 1;
				}
				else{
					triggerButtonLock = 0;
					deviceStatus.isLocked ^= 1;
					beep(1);
					delay_ms(250);
					beep(1);
				}
			}
		}
		
		if(!!deviceStatus.isLocked){
			delay_ms(1000);
			continue;
		}
			
		
		checkSlider(&allButtonsRAW);
		
		if(ds18b20.isSensorError){//Если датчик температуры не подключен
			do{
				printTempSensorError();
				tempSensorInit();
				}while(ds18b20.isSensorError);//Ждем пока подключат датчик
				initMenuMainPage();//Перезапускаем меню
		}
		
		buttonNavigationFunction();//Функции кнопок в соответствии с ситуйцией
		//menuNavigation(allButtonsRAW & BUTTON_UP, allButtonsRAW & BUTTON_DN, allButtonsRAW & BUTTON_OK);
//		
		if(millis() - readTemperatureTimer > 1000){
			readTemperatureTimer = millis();
			readTemperature();
			deviceStatus.temperatureCurrent = ds18b20.temperature;
		}

	
		mainProcess();//Указатель на текущий процесс
		if(!!deviceStatus.pidEnable){
			pid_relay_control(&pid);
		}
}
}
// 0x0800 0x0400 0x0200 0x0080 0x0040 0x0020
/**/


/*
Функция кнопок - переключение мунктом меню
*/
void menuNavigationFunction(void){
menuNavigation(allButtonsRAW & BUTTON_UP, allButtonsRAW & BUTTON_DN, allButtonsRAW & BUTTON_OK);

}



