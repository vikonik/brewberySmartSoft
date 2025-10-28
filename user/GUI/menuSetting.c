#include "menuSetting.h"
#include "manifest.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mainPage.h"
#include "microsoftSansSerif_8ptFont.h"
#include "allDefenition.h"
#include "delay.h"

	extern uint16_t allButtonsRAW;
	extern menu_t menu_Recept;
	extern void (*buttonNavigationFunction)(void);

void menuNavigationFunction(void);
void menuSetTimeOK(void);

void menuChangeTimesetButtonNavigationFunction(void);

static const uint8_t firstStringH = 15;
MenuText_t label_SetTime = {stringTime, 1, firstStringH+ charHeight8pt * 0};
MenuText_t label_WiFi = {stringWiFiOff, 1, firstStringH+ charHeight8pt * 1};
MenuText_t label_BT = {stringBtOn, 1, firstStringH+ charHeight8pt * 2};
MenuText_t label_Mute = {stringMuteOff, 1, firstStringH+ charHeight8pt * 3};
MenuText_t label_SettingNazad = {stringNazad, 80, firstStringH+ charHeight8pt * 3};


MENU_ADD(menu_SetTime,			menu_WiFi,  				m_null,    			m_null,      printChangeTime, NULL,  		&label_SetTime);
MENU_ADD(menu_WiFi,					menu_BT,  					menu_SetTime,   m_null,      cahageWiFiState, NULL,    	&label_WiFi);
MENU_ADD(menu_BT,						menu_Mute,  				menu_WiFi,    	m_null,      changeBtState,   NULL,    	&label_BT);
MENU_ADD(menu_Mute,					menu_SettingNazad, 	menu_BT,    		m_null,      changeMuteState,   NULL,    	&label_Mute);
MENU_ADD(menu_SettingNazad, m_null,  						menu_Mute,  		menu_Recept, allMenuTextClear,NULL,    	&label_SettingNazad);


/**/

/**/
void printMenuSetting(void){
	deviceStatus.isPowerSetting = 0;
    allMenuTextClear();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
	
		if(deviceStatus.wifiStatus == 0){
			menu_WiFi.text->text = stringWiFiOn;
		}
		else{
			menu_WiFi.text->text = stringWiFiOff;
		}
		
		if(deviceStatus.btStatus == 0){
			menu_BT.text->text = stringBtOn;
		}
		else{
			menu_BT.text->text = stringBtOff;
		}	
		
    MENU_Init(&menu_SetTime);
}

/**/
void cahageWiFiState(void){
allMenuTextClear();
	if(deviceStatus.wifiStatus == 1){//Выключаем WiFi
			menu_WiFi.text->text = stringWiFiOn;//Устанавливаем новую надпись
			deviceStatus.wifiStatus = 0;//Меняем состояние
		}
		else{
			menu_WiFi.text->text = stringWiFiOff;//Устанавливаем новую надпись
			deviceStatus.wifiStatus = 1;//Меняем состояние
			//Переключаем блютуз
			menu_BT.text->text = stringBtOn;//Устанавливаем новую надпись
			deviceStatus.btStatus = 0;//Меняем состояние
		}
		MENU_Init(&menu_WiFi);
}

/**/
void changeBtState(void){
allMenuTextClear();
	if(deviceStatus.btStatus == 1){//Выключаем BT
			menu_BT.text->text = stringBtOn;//Устанавливаем новую надпись
			deviceStatus.btStatus = 0;//Меняем состояние
		}
		else{
			menu_BT.text->text = stringBtOff;//Устанавливаем новую надпись
			deviceStatus.btStatus = 1;//Меняем состояние
			menu_WiFi.text->text = stringWiFiOn;//Устанавливаем новую надпись
			deviceStatus.wifiStatus = 0;//Меняем состояние
		}
		MENU_Init(&menu_BT);
}

/**/
void changeMuteState(void){
allMenuTextClear();
	if(!!deviceStatus.isMuted){//Выключаем BT
			menu_Mute.text->text = stringMuteOff;//Устанавливаем новую надпись
			deviceStatus.isMuted = 0;//Меняем состояние
		}
		else{
			menu_Mute.text->text = stringMuteOn;//Устанавливаем новую надпись
			deviceStatus.isMuted = 1;//Меняем состояние
		}
		MENU_Init(&menu_Mute);
}

/********************** Установка времени **************************/
static MenuText_t label_timeH = 		{NULL, 49, firstStringH + charHeight8pt * 1};
static MenuText_t label_timeM = 		{NULL, 64, firstStringH + charHeight8pt * 1};
static MenuText_t label_timeNazad = {stringNazad, 1, firstStringH + charHeight8pt * 3};
static MenuText_t label_timeOK = 		{stringOK, 100, firstStringH + charHeight8pt * 3};

uint8_t maxH = 24;
uint8_t maxM = 60;
MENU_ADD(menu_SetH,					menu_SetM,  				m_null,    					m_null,      menuChangeTimesetButtonNavigationFunction,   					&maxH,  		&label_timeH);
MENU_ADD(menu_SetM,					menu_SetTimeNazad,  menu_SetH,    			m_null,      menuChangeTimesetButtonNavigationFunction,   					&maxM,  		&label_timeM);
MENU_ADD(menu_SetTimeNazad,	menu_SetTimeOK,  		menu_SetM,    			m_null,      printMenuSetting,   					NULL,  		&label_timeNazad);
MENU_ADD(menu_SetTimeOK,		m_null,  						menu_SetTimeNazad,  m_null,      menuSetTimeOK,   					NULL,  		&label_timeOK);


void printChangeTime(void){
	static char _bufH[4];
	static char _bufM[4];
	uint64_t timeRaw = getGlobalTime();
	timeRaw /= 60000; //это столько натикало минут
	uint8_t hh = timeRaw / 60;
	hh  %= 24;
	uint8_t mm = timeRaw % 60;
	allMenuTextClear();
	sprintf(_bufH, "%02d", hh);
	label_timeH.text = _bufH;
	sprintf(_bufM, "%02d", mm);
	label_timeM.text = _bufM;	
	
	ST7567_FB_printText(label_timeH.posX+13, label_timeH.posY, ":", NORMAL);
	ST7567_FB_display();
	MENU_Init(&menu_SetH);
}

/**/
static void changeValueTime(uint16_t* _allButtonsRAW, MenuText_t* time, uint8_t maxValue){
	
	static uint64_t blincTimer = 0;
static uint64_t valueTimer = 0;
	uint8_t data = 0;
	data = (time->text[0] - '0') * 10 + (time->text[1] - '0');
//	static char _buf[8];
	static uint8_t trigColor = 0;
	static uint8_t trigColorCnt = 0;
	static uint16_t buttonDelay = 100;
	static uint8_t buttonPressCounter = 0;
	static uint8_t flagChange = 0;
	
	if(millis() - blincTimer > 100){
		blincTimer = millis();
	//	sprintf(buf, "%02.1f", manualControl.targetTemperature);
	//	sprintf(_buf, "OK");

		ST7567_FB_fillRect(time->posX, time->posY, 12, 13, 0);
		if(!!flagChange)
			ST7567_FB_printText(time->posX, time->posY, (char*)time->text, INVERSE);
  	else
		 ST7567_FB_printText(time->posX, time->posY, (char*)time->text, (Inverse_t)trigColor);//	SWITCH
		if(trigColorCnt == 0)
			trigColor ^= 1;
		trigColorCnt++;
		trigColorCnt %= 5;
		
		ST7567_FB_display();	
	}
	
	if(buttonPressCounter == 1)
		buttonDelay = 500;
	else if(buttonPressCounter >= 6)
		buttonDelay = 250;
	else
		buttonDelay = 100;
	
		if(millis() - valueTimer > buttonDelay){
			valueTimer = millis();
			if(*_allButtonsRAW & BUTTON_UP){
				flagChange = 1;
				if(data < maxValue-1)
					data += 1;
				else
					data = 0;
				
				if(buttonPressCounter < 6)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_DN){
				flagChange = 1;
				if(data > 0 )
					data -= 1;
				else
					data = maxValue  - 1;
					
				if(buttonPressCounter < 6)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_OK){
				//Сохраняем данные в стоковую переменную
				sprintf((char*)time->text, "%02d", data);
				buttonNavigationFunction = menuNavigationFunction;
				MENU_Init(m_curr);
			}			
			else{
				buttonPressCounter = 0;
				flagChange = 0;
			} 
			
			sprintf((char*)time->text, "%02d", data);
		}
}

/**/
void menuSetTimeOK(void){
	uint8_t timeH = 0;
	uint8_t timeM = 0;	
	uint64_t globalTime;
	timeH = (label_timeH.text[0] - '0') * 10 + (label_timeH.text[1] - '0');
	timeM = (label_timeM.text[0] - '0') * 10 + (label_timeM.text[1] - '0');
	setGlobalTime((timeH*60 + timeM)*60000);
	
}

/**/
void menuChangeTimeFunctionTime(void){
	changeValueTime(&allButtonsRAW, m_curr->text, *m_curr->data);
}

/*
Устанавливаем функцию на которуюу будут переключены кнопки
*/
void menuChangeTimesetButtonNavigationFunction(void){
buttonNavigationFunction = menuChangeTimeFunctionTime;
	allButtonsRAW = 0;//Сбросили состояние кнопок
}
