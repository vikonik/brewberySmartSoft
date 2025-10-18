/*
Это шаблон для вывода информации о состоянии текущего процесс
*/


#include "menuReceptShablon.h"
#include "menu.h"
#include "allDefenition.h"
#include <stdio.h>
#include "mainPage.h"//allMenuTextClear()
#include "manifest.h"
#include "delay.h"
#include "HAL.h"
#include "pid.h"
#include "recept.h"
#include "state_machine.h"
#include "state_machine.h"

extern Recipe_t current_recipe;
extern uint8_t current_mash_stage;
extern void (*buttonNavigationFunction)(void);
extern void (*mainProcess)(void);
	void functionNull(void);
	extern uint16_t allButtonsRAW;
	
void menuChangeDataFunctionTemp(void);
void menuChangeDataFunctionTimer(void);
void menuNavigationFunction(void);	
void setMenuNavigationToChangeValue(void);
void shablonSetFunction(void);
void shablonExecute(void);

void (*shablonExecuteSetFunction)(void);
static const uint8_t firstStringH = 15;


char *currentProcessBuf;//[16];//Назваие текущего процесса атоматического нецепта
ShablonlControl_t receptControl[5];
//Текст таблицы
MenuText_t label_Temperature = {stringTemperature, 1, firstStringH + charHeight8pt * 1};
MenuText_t label_Time = {stringTime, 1, firstStringH + charHeight8pt * 2};
MenuText_t label_Ustavka = {stringUstavka, 55, firstStringH + charHeight8pt * 0};
MenuText_t label_Sostoianie = {stringSostoianie, 100, firstStringH + charHeight8pt * 0};

//Пункты меню
MenuText_t label_ShablonTemperature = {NULL, 55, firstStringH + charHeight8pt * 1};
MenuText_t label_ShablonTimer = {NULL, 42, firstStringH + charHeight8pt * 2};
MenuText_t label_ShablonNazad = {stringNazad, 45, firstStringH + charHeight8pt * 3};
MenuText_t label_ShablonState = {stringPusk, 100, firstStringH + charHeight8pt * 3};
MenuText_t label_ShablonSkip = {stringSkip, 40, firstStringH + charHeight8pt * 3};

ManualControl_e shablonTemperatureData;
ManualControl_e shablonTimeData;

uint8_t menu_ShablonState_data = 0;
MENU_ADD(menu_ShablonTempersture,		menu_ShablonTime,  	m_null,             			m_null,  setButtonNavigationToChangeValueTemp, 	&shablonTemperatureData, 	&label_ShablonTemperature);
MENU_ADD(menu_ShablonTime,					menu_ShablonNazad,  menu_ShablonTempersture,  m_null,  setButtonNavigationToChangeValueTimer, &shablonTimeData, 				&label_ShablonTimer);
MENU_ADD(menu_ShablonNazad,					menu_ShablonState,  menu_ShablonTime,         m_null,  NULL/*printMenuManualControl*/,   			NULL, 										&label_ShablonNazad);
MENU_ADD(menu_ShablonState,					m_null,  						menu_ShablonNazad,        m_null,  shablonSetFunction,   									&menu_ShablonState_data, 										&label_ShablonState);

MENU_ADD(menu_ShablonSkip,					m_null,  						m_null,        						m_null,  switchStadge,   												NULL, 										&label_ShablonSkip);

//Позиции для вывода температуры и времени
MenuText_t label_ShablonCurentTemperature = {NULL, 100, firstStringH + charHeight8pt * 1};
MenuText_t label_ShablonCurrentTime = {NULL, 79, firstStringH + charHeight8pt * 2};
/*
menu - menu_ShablonTempersture
*/
char* getTemperatureStr(ShablonlControl_t* control, uint8_t addr ) {
    static char buf[8];
    sprintf(buf, "%02.1f", control[addr].targetTemperature);
    return buf;
}

/*
menu - menu_ShablonTime
*/
char* getTimerStr(ShablonlControl_t* control, uint8_t addr) {
    static char buf[8];
    sprintf(buf,"%03d:%02d",control[addr].targetTimer_h, control[addr].targetTimer_m);
    return buf;
}
/*
control - 
menuTemperature
menuTime
labelTime
*/

void printShablon(ShablonlControl_t* control, menu_t *menuTemperature,  menu_t *menuTime)/*, MenuText_t* labelTemperatire, MenuText_t* labelTime)*/{
//	char buf[8];
//	const uint8_t temperaturePosX = 60;
//	const uint8_t temperaturePosY = label_Temperature.posY;	
//	const uint8_t timerPosX = 45;
//	const uint8_t timerPosY = label_Time.posY;	
	//Определяем какую функцию вызывам
	menuTemperature->data = m_curr->data;
	menuTime->data = m_curr->data;	
	
	allMenuTextClear();
//	ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
	ST7567_FB_printText(label_Ustavka.posX, label_Ustavka.posY, (char*)label_Ustavka.text, NORMAL);
	ST7567_FB_printText(label_Sostoianie.posX, label_Sostoianie.posY, (char*)label_Sostoianie.text, NORMAL);	
	ST7567_FB_printText(label_Temperature.posX, label_Temperature.posY, (char*)label_Temperature.text, NORMAL);		
	ST7567_FB_printText(label_Time.posX, label_Time.posY, (char*)label_Time.text, NORMAL);
//	ST7567_FB_fillRect(1, 1, 48, 13, 0);
	switch(*m_curr->data){
		case 	MESH:
			ST7567_FB_printText(1, 1, (char*)_stringMesh, NORMAL);
			break;
		case 	FILTRATION: 
			ST7567_FB_printText(1, 1, (char*)_stringFiltracion, NORMAL);
			break;
		case 	NAGREV: 
			ST7567_FB_printText(1, 1, (char*)_stringNagrev, NORMAL);
			break;
		case 	COLD: 
			ST7567_FB_printText(1, 1, (char*)_stringСold, NORMAL);
			break;
		case 	FERMENTATION: 
			ST7567_FB_printText(1, 1, (char*)_stringFermentation, NORMAL);
			break;
	}
		
menuTemperature->text->text = getTemperatureStr(control, *menuTemperature->data);
menuTime->text->text = getTimerStr(control, *menuTime->data);
	//labelTemperatire->text = getTemperatureStr(control, menuTemperature);
	//labelTime->text = getTimerStr(control, menuTime);
	
	MENU_Init(menuTemperature);
	
}

/**/
void changeValue(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data){
	static uint64_t blincTimer = 0;
	static uint64_t valueTimer = 0;	
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
		label_ShablonTemperature.text = getTemperatureStr(control,data);
		ST7567_FB_fillRect(label_ShablonTemperature.posX, label_ShablonTemperature.posY, 32, 13, 0);
		if(!!flagChange)
			ST7567_FB_printText(label_ShablonTemperature.posX, label_ShablonTemperature.posY, (char*)label_ShablonTemperature.text, INVERSE);
  	else
		 ST7567_FB_printText(label_ShablonTemperature.posX, label_ShablonTemperature.posY, (char*)label_ShablonTemperature.text, (Inverse_t)trigColor);//	SWITCH
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
				control[data].targetTemperature += 1;
				if(buttonPressCounter < 6)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_DN){
				flagChange = 1;
				control[data].targetTemperature -= 1;
				if(buttonPressCounter < 6)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_OK){
				//Сохраняем данные в рабочую структуру
				deviceStatus.manualControlCurrentData.targetTemperature = control->targetTemperature;
				pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
				setMenuNavigationToChangeValue();
				MENU_Init(&menu_ShablonTempersture);
			}			
			else{
				buttonPressCounter = 0;
				flagChange = 0;
			} 
		}
}


/**/
void changeValueTime(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data){
	static uint64_t blincTimer = 0;
	static uint64_t valueTimer = 0;	
//	static char _buf[8];
	static uint8_t trigColor = 0;
	static uint8_t trigColorCnt = 0;
	static uint16_t buttonDelay = 100;
	static uint8_t buttonPressCounter = 0;
	static uint8_t flagChange = 0;
	static uint64_t _time = 0;
	_time = control[data].targetTimer_h*60 + control[data].targetTimer_m;
	if(millis() - blincTimer > 100){
		blincTimer = millis();
		label_ShablonTimer.text = getTimerStr(control,data);
		ST7567_FB_fillRect(label_ShablonTimer.posX, label_ShablonTimer.posY, 32, 13, 0);
		if(!!flagChange)
			ST7567_FB_printText(label_ShablonTimer.posX, label_ShablonTimer.posY, (char*)label_ShablonTimer.text, INVERSE);
  	else
		 ST7567_FB_printText(label_ShablonTimer.posX, label_ShablonTimer.posY, (char*)label_ShablonTimer.text, (Inverse_t)trigColor);//	SWITCH
		if(trigColorCnt == 0)
			trigColor ^= 1;
		trigColorCnt++;
		trigColorCnt %= 5;
		
		ST7567_FB_display();	
	}
	
	if(buttonPressCounter == 1)
		buttonDelay = 500;
	else if(buttonPressCounter >= 90)
		buttonDelay = 252;	
	else if(buttonPressCounter >= 46)
		buttonDelay = 251;
	else if(buttonPressCounter >= 6)
		buttonDelay = 250;
	else
		buttonDelay = 1000;
	
		if(millis() - valueTimer > buttonDelay){
			valueTimer = millis();
			if(*_allButtonsRAW & BUTTON_UP){
				
				flagChange = 1;
				if(_time >= 59940)//999:99 в минутах
					_time = 0xFFFFFFFFFFFFFFFF;
				
					if(buttonDelay == 250)
						_time += 10;
					if(buttonDelay == 251)
						_time += 60;	
					if(buttonDelay == 252)
						_time += 600;						
					else 
						_time += 1;
			  
				if(buttonPressCounter < 90)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_DN){
				flagChange = 1;

				//При подходе к 0 сбавляем скорость
				if(_time < 10){
					buttonDelay = 1;
					buttonPressCounter = 0;
				}
					
				
					if(buttonDelay == 250)
						_time -= 10;
					if(buttonDelay == 251)
						_time -= 60;	
					if(buttonDelay == 252)
						_time -= 600;						
					else 
						_time -= 1;
			  
				if(_time > 59940)
					_time = 59940;
				if(buttonPressCounter < 90)
								buttonPressCounter++;
			}
			else if(*_allButtonsRAW & BUTTON_OK){
				//Сохраняем данные в рабочую структуру
				control[data].targetTimer_h = _time / 60; 
				control[data].targetTimer_m = _time % 60; 
				deviceStatus.manualControlCurrentData.targetTimer_h =  control[data].targetTimer_h;//Дублируем пересчет
				deviceStatus.manualControlCurrentData.targetTimer_m =  control[data].targetTimer_m; 
				deviceStatus.manualControlCurrentData.targetTimer_s = 0;
				setMenuNavigationToChangeValue();
				MENU_Init(&menu_ShablonTime);
				return;
			}			
			else{
				buttonPressCounter = 0;
				flagChange = 0;
			} 
		}
		control[data].targetTimer_h = _time / 60; 
		control[data].targetTimer_m = _time % 60; 
}
/**/
void setButtonNavigationToChangeValueTemp(void){
	allButtonsRAW = 0;
	buttonNavigationFunction = menuChangeDataFunctionTemp;
}

/**/
void setButtonNavigationToChangeValueTimer(void){
	allButtonsRAW = 0;
	buttonNavigationFunction = menuChangeDataFunctionTimer;
}

/**/
void menuChangeDataFunctionTemp(void){
	changeValue(&allButtonsRAW, receptControl,*m_curr->data);
}
 
/**/
void menuChangeDataFunctionTimer(void){
	changeValueTime(&allButtonsRAW, receptControl, *m_curr->data);
}

/**/
void setMenuNavigationToChangeValue(void){
	buttonNavigationFunction = menuNavigationFunction;
}
/*
Кнопка Пуск/Стоп
*/
void shablonSetFunction(void){

	if(!deviceStatus.flagRegimOn){
		deviceStatus.flagRegimOn = 1;
		mainProcess = shablonExecute;
		label_ShablonState.text = stringStop;//Меняем надпись
		//Блокируем функцию Назад
		menu_ShablonTime.next = &menu_ShablonState;				
		menu_ShablonState.previous = &menu_ShablonTime;
		//Сотрем надпись
		ST7567_FB_fillRect(label_ShablonNazad.posX, label_ShablonNazad.posY, 45, 13, 0);	
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[*menu_ShablonTime.data].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[*menu_ShablonTime.data].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
		deviceStatus.pidEnable = 1;
	}
	else{
		deviceStatus.flagRegimOn = 0;
		ST7567_FB_fillRect(	label_ShablonState.posX, label_ShablonState.posY, 
											128-label_ShablonCurentTemperature.posX, 13, 0);
		label_ShablonState.text = stringPusk;//Меняем надпись
		//Возвращаем функцию Назад
		menu_ShablonTime.next = &menu_ShablonNazad;				
		menu_ShablonState.previous = &menu_ShablonNazad;
		mainProcess = functionNull;//Отключаем функцию
		deviceStatus.pidEnable = 0;
	}

	MENU_Init(&menu_ShablonState);
}

/*
Выводим на дисплей текущие данные 
*/
void shablonPrintCurrentData(void){
	ST7567_FB_fillRect(	label_ShablonCurentTemperature.posX, label_ShablonCurentTemperature.posY, 
											128-label_ShablonCurentTemperature.posX, 24, 0);
	ST7567_FB_printText(label_ShablonCurentTemperature.posX, label_ShablonCurentTemperature.posY, (char*)label_ShablonCurentTemperature.text, NORMAL);
	
	ST7567_FB_printText(label_ShablonCurrentTime.posX, label_ShablonCurrentTime.posY, (char*)label_ShablonCurrentTime.text, NORMAL);

}

/*
Выполняем действия в соответствии с выбранным шаблоном
*/
void shablonExecute(void){
	  static char bufTemp[8];
		static char bufTime[8];
		static uint64_t shablonExecutePeriod = 0;
		static uint64_t curentTic = 0;
	
	curentTic = millis();
	if(curentTic - shablonExecutePeriod > 1000){
		
    sprintf(bufTemp, "%02.1f", deviceStatus.temperatureCurrent);
		label_ShablonCurentTemperature.text = bufTemp;

// Уменьшаем секунды
    if (deviceStatus.manualControlCurrentData.targetTimer_s > 0) {
        deviceStatus.manualControlCurrentData.targetTimer_s--;
    } else {
        // Если секунды закончились, уменьшаем минуты
        deviceStatus.manualControlCurrentData.targetTimer_s = 59;
        
        if (deviceStatus.manualControlCurrentData.targetTimer_m > 0) {
            deviceStatus.manualControlCurrentData.targetTimer_m--;
        } else {
            // Если минуты закончились, уменьшаем часы
            deviceStatus.manualControlCurrentData.targetTimer_m = 59;
            
            if (deviceStatus.manualControlCurrentData.targetTimer_h > 0) {
                deviceStatus.manualControlCurrentData.targetTimer_h--;
            } else {
                // Таймер завершен
                deviceStatus.manualControlCurrentData.targetTimer_h = 0;
                deviceStatus.manualControlCurrentData.targetTimer_m = 0;
                deviceStatus.manualControlCurrentData.targetTimer_s = 0;
                
                // Здесь можно вызвать callback или установить флаг события
                //timer_finished_callback();
							deviceStatus.pidEnable = 0;//Останавливаем 
							shablonSetFunction();
							for(uint8_t i = 0; i < 10; i++){
								beep(1);
								delay_ms(100);
							}
							
            }
        }
    }
    
		
    sprintf(bufTime,"%03d:%02d:%02d",	deviceStatus.manualControlCurrentData.targetTimer_h, 
																			deviceStatus.manualControlCurrentData.targetTimer_m, 
																			deviceStatus.manualControlCurrentData.targetTimer_s);
		label_ShablonCurrentTime.text = bufTime;
		shablonPrintCurrentData();	
shablonExecutePeriod = curentTic;		
	}

}

/*
устанавливаем температуру через удаленное соединение
*/
void distanceSetTemperature(float newTemperature, void(*func)(void)){
	if(m_curr != &menu_ShablonTempersture && m_curr != &menu_ShablonTime && m_curr != &menu_ShablonNazad && m_curr != &menu_ShablonState)
		return;

	manualControl[*m_curr->data].targetTemperature = newTemperature;
	deviceStatus.manualControlCurrentData.targetTemperature = manualControl[*m_curr->data].targetTemperature;
	pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	func();
	//printShablonManalControl();
}
/*
устанавливаем время через удаленное соединение
*/
void distanceSetTimer(uint16_t newTimer, void(*func)(void)){
	if(m_curr != &menu_ShablonTempersture && m_curr != &menu_ShablonTime && m_curr != &menu_ShablonNazad && m_curr != &menu_ShablonState)
		return;
	manualControl[*m_curr->data].targetTimer_h = newTimer / 60; 
	manualControl[*m_curr->data].targetTimer_m = newTimer % 60; 
	deviceStatus.manualControlCurrentData.targetTimer_h =  manualControl[*m_curr->data].targetTimer_h;//Дублируем пересчет
	deviceStatus.manualControlCurrentData.targetTimer_m =  manualControl[*m_curr->data].targetTimer_m; 
	deviceStatus.manualControlCurrentData.targetTimer_s = 0;
	//printShablonManalControl();
	func();
}
 
/*************************************************************/
