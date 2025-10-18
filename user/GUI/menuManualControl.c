#include "menuManualControl.h"
#include "allDefenition.h"
#include "ST7567_FB.h"
#include <stdio.h>
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"
#include "menu.h"
#include "mainPage.h"
#include "ST7567_FB.h"
#include "delay.h"
#include "HAL.h"
#include "pid.h"
#include "menuReceptShablon.h"

extern void (*shablonExecuteSetFunction)(void);
extern void (*buttonNavigationFunction)(void);
extern void (*mainProcess)(void);
void menuChangeDataFunctionTemp(void);
void menuChangeDataFunctionTimer(void);
void menuNavigationFunction(void);

void shablonSetFunctionManualControl(void);

static const uint8_t firstStringH = 15;

	void functionNull(void);
	extern uint16_t allButtonsRAW;
	extern MenuText_t label_ShablonState, label_ShablonNazad, label_ShablonCurentTemperature;
extern ShablonlControl_t receptControl[];
ShablonlControl_t manualControl[5] = {
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
    },
		    {
        .targetTemperature = 0.0,
        .targetTimer_h = 0,
        .targetTimer_m = 0,
        .state = 0
    }
};
//void (*shablonFunction[5])(void) = {NULL, NULL, NULL, NULL, NULL};//Массив для хранения указателей на функции каждого процесса

MenuText_t label_Mesh = {stringMesh, 1, firstStringH+ charHeight8pt * 0};
MenuText_t label_Filtracion = {stringFiltracion, 1, firstStringH + charHeight8pt * 1};
MenuText_t label_Nagrev = {stringNagrev, 1, firstStringH+ charHeight8pt * 2};
MenuText_t label_Cold = {stringСold, 1, firstStringH+ charHeight8pt * 3};
MenuText_t label_Fermentation = {stringFermentation, 66, firstStringH+ charHeight8pt * 0};


ManualControl_e mesh = MESH;
ManualControl_e nagrev = NAGREV;
ManualControl_e cold = COLD;
ManualControl_e ferm = FERMENTATION;
ManualControl_e filt = FILTRATION;

MENU_ADD(menu_Mesh,					  menu_Filtracion,  	m_null,    				m_null, printShablonManalControl,   &mesh,    &label_Mesh);
MENU_ADD(menu_Filtracion,			menu_Nagrev,  			menu_Mesh,				m_null, printShablonManalControl, 	&filt,   	&label_Filtracion);
MENU_ADD(menu_Nagrev,					menu_Cold,  				menu_Filtracion,  m_null, printShablonManalControl,   &nagrev,  &label_Nagrev);
MENU_ADD(menu_Cold,						menu_Fermentation,  menu_Nagrev,    	m_null, printShablonManalControl,   &cold,    &label_Cold);
MENU_ADD(menu_Fermentation,		menu_Nazad,  				menu_Cold,        m_null, printShablonManalControl,   &ferm,    &label_Fermentation);



/**/
void printMenuManualControl(void){
    allMenuTextClear();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_Fermentation;
		menu_Nazad.select = &menu_Recept;
		menu_Nazad.func = allMenuTextClear;
		menu_Nazad.text->text = stringNazad;
		menu_Nazad.text->posY = firstStringH + charHeight8pt*3;
		menu_Nazad.text->posX = 95;
    MENU_Init(&menu_Mesh);
	
	
	//shablonExecuteSetFunction = shablonSetFunctionManualControl;
}

/*
Кнопка Пуск/Стоп
*/
void shablonSetFunctionManualControl(void){

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
		deviceStatus.manualControlCurrentData.targetTimer_h =  manualControl[*menu_ShablonTime.data].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  manualControl[*menu_ShablonTime.data].targetTimer_m; 
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

}
	


/**/
void printShablonManalControl(void){
	
	    //ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_ShablonNazad.next = &menu_ShablonState;
		menu_ShablonNazad.previous = &menu_ShablonTime;
		menu_ShablonNazad.select = &menu_Mesh;
		//menu_ShablonNazad.func = allMenuTextClear;
		menu_ShablonNazad.text->text = stringNazad;
		menu_ShablonNazad.text->posY = firstStringH + charHeight8pt*3;
		menu_ShablonNazad.text->posX = 45;
	
	
	menu_ShablonState.func = shablonSetFunction;
	//Переопределяем пункт меню menu_ShablonState
	menu_ShablonState.previous = &menu_ShablonNazad;
	

	menu_ShablonNazad.func = printMenuManualControl;
	
	//menu_ShablonState.func = shablonSetFunctionManualControl;
	//printShablon(receptControl, &menu_ShablonTempersture, &menu_ShablonTime, &label_ShablonTemperature, &label_ShablonTimer);

	printShablon(receptControl, &menu_ShablonTempersture, &menu_ShablonTime);
}











