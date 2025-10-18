#include "menuReceptLoaded.h"
#include "menuReceptShablon.h"
#include "mainPage.h"
#include "manifest.h"
#include "menu.h"
#include "recept.h"
#include "state_machineLoaded.h"
#include <stdio.h>
#include "pid.h"
#include "delay.h"
#include "recipe_manager.h"//Загрудка из Flash

/*************************** Функции внутренние *************************/
void printReceptLoaded(void);
void setButtonNavigationToChangeValueTempRecepteLoaded(void);
void setButtonNavigationToChangeValueTimerRecepteLoaded(void);
void setMenuNavigationToChangeValueRecepteLoaded(void);
void receptSetFunctionLoaded(void);
void printShablonRecepteLoaded(Recipe_t *recept, SystemState_t* processState);
/*************************************************************************/
/***************************** Переменные внутренние ***********************/
static const uint8_t firstStringH = 15;

extern menu_t menu_ReceptLoaded_1;
extern uint16_t allButtonsRAW;

	//Надписи берем из шаблона
	//Текст таблицы
extern MenuText_t label_Temperature;
extern MenuText_t label_Time;
extern MenuText_t label_Ustavka ;
extern MenuText_t label_Sostoianie ;

//Пункты меню
extern MenuText_t label_ShablonTemperature;
extern MenuText_t label_ShablonTimer ;
extern MenuText_t label_ShablonNazad ;
extern MenuText_t label_ShablonState ;
extern MenuText_t label_ShablonSkip ;

extern MenuText_t label_ShablonCurentTemperature ;
extern MenuText_t label_ShablonCurrentTime ;

static uint8_t recepteTemperatureData;
static uint8_t recepteTimeData;
static uint8_t menu_RecepteState_data;

extern ShablonlControl_t receptControl[];

extern void (*mainProcess)(void);
extern void (*buttonNavigationFunction)(void);
extern void menuNavigationFunction(void);	
extern void functionNull(void);
extern Recipe_t current_recipe;


MENU_ADD(menu_RecepteLoadedTempersture,		menu_RecepteLoadedTime,  	m_null,             						m_null,  setButtonNavigationToChangeValueTempRecepteLoaded, 	&recepteTemperatureData, 	&label_ShablonTemperature);
MENU_ADD(menu_RecepteLoadedTime,					menu_RecepteLoadedNazad,  menu_RecepteLoadedTempersture,  m_null,  setButtonNavigationToChangeValueTimerRecepteLoaded, 	&recepteTimeData, 				&label_ShablonTimer);
MENU_ADD(menu_RecepteLoadedNazad,					menu_RecepteLoadedState,  menu_RecepteLoadedTime,         m_null,  printReceptLoaded,   																NULL, 										&label_ShablonNazad);
MENU_ADD(menu_RecepteLoadedState,					m_null,  									menu_RecepteLoadedNazad,        m_null,  receptSetFunctionLoaded,   													&menu_RecepteState_data, 	&label_ShablonState);
MENU_ADD(menu_RecepteLoadedSkip,					m_null,  									m_null,        									m_null,  switchStadge,   																			NULL, 										&label_ShablonSkip);


void startRecepteProcessLoaded(void){
//Загружаем рецепт из памяти
	uint8_t receptIndex = *m_curr->data;//Здесь m_cut пока еще тот рецепт по которому перешли
	Recipe_ReadFromFlash(&current_recipe, receptIndex);
	current_mash_stage_Loaded = 0;
	//Выводим измененое меню на экран 
	printShablonRecepteLoaded((Recipe_t*)&current_recipe, &current_state);

 
} 
	/*
	Уходим на выбор рецептов
	*/
	void printReceptLoaded(void){
		allMenuTextClear();
		MENU_Init(&menu_ReceptLoaded_1);
	}
	/**/
void changeReceptValueTemperatureLoaded(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data){
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
				void setButtonNavigationToChangeValueTimerRecepte(void);
				setMenuNavigationToChangeValueRecepteLoaded();
				MENU_Init(&menu_RecepteLoadedTempersture);
			}			
			else{
				buttonPressCounter = 0;
				flagChange = 0;
			} 
		}
}


/**/
void changeReceptValueTimeLoaded(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data){
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
				setMenuNavigationToChangeValueRecepteLoaded();
				MENU_Init(&menu_RecepteLoadedTime);
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
void menuChangeDataFunctionTempRecepteLoaded(void){
	changeReceptValueTemperatureLoaded(&allButtonsRAW, receptControl, 0);
}
 
/**/
void menuChangeDataFunctionTimerRecepteLoaded(void){
	changeReceptValueTimeLoaded(&allButtonsRAW, receptControl, 0);
}
/**/
void setButtonNavigationToChangeValueTempRecepteLoaded(void){
	allButtonsRAW = 0;
	buttonNavigationFunction = menuChangeDataFunctionTempRecepteLoaded;
}

/**/
void setButtonNavigationToChangeValueTimerRecepteLoaded(void){
	allButtonsRAW = 0;
	buttonNavigationFunction = menuChangeDataFunctionTimerRecepteLoaded;
}

/**/
void setMenuNavigationToChangeValueRecepteLoaded(void){
	buttonNavigationFunction = menuNavigationFunction;
}

/**/
void printMenuReceptControlLoad(void){
    allMenuTextClear();
    //ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		//menu_ShablonNazad.next = &menu_ShablonSkip;
		menu_ShablonNazad.next = &menu_ShablonState;	
		menu_ShablonNazad.previous = &menu_ShablonTime;
		menu_ShablonNazad.select = &m_null;
		menu_ShablonNazad.func = allMenuTextClear;
		menu_ShablonNazad.text->text = stringNazad;
		menu_ShablonNazad.text->posY = firstStringH + charHeight8pt*3;
		menu_ShablonNazad.text->posX = 1;
	
//	//Добавляем пункт пропустиить
//		menu_ShablonSkip.next = &menu_ShablonState;
//		menu_ShablonSkip.previous = &menu_ShablonNazad;
//	
//	
//	//Переопределяем пункт меню menu_ShablonState
//	menu_ShablonState.previous = &menu_ShablonSkip;
	
	
    MENU_Init(&menu_ShablonTempersture);
}

/*
Запускаем автоматический процесс
*/
void receptSetFunctionLoaded(void){

	if(!deviceStatus.flagRegimOn){
		deviceStatus.flagRegimOn = 1;
		mainProcess = StateMachine_Process_Loaded;
		StateMachine_Process_Start_Loaded();//Запустили процесс
	//	mainProcess = shablonExecute;//Процесс который будет запускаться в основном цикле
		label_ShablonState.text = stringStop;//Меняем надпись
		//Блокируем функции Назад и пропустить
		menu_RecepteLoadedTime.next = 			&menu_RecepteLoadedState;				
		menu_RecepteLoadedState.previous = 	&menu_RecepteLoadedTime;
		
		
		//Сотрем надпись Дальше и Назад
		ST7567_FB_fillRect(label_ShablonNazad.posX, label_ShablonNazad.posY, 90, 13, 0);	
		
		//пишем название процесса
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
		ST7567_FB_printText(1, 1, (char*)_stringMesh, NORMAL);
		//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = current_recipe.mash_stages[current_mash_stage_Loaded].time / 60;
		receptControl[0].targetTimer_m = current_recipe.mash_stages[current_mash_stage_Loaded].time % 60;
		receptControl[0].targetTemperature = current_recipe.mash_stages[current_mash_stage_Loaded].temperature/10;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
		deviceStatus.pidEnable = 1;
	}
	else{
		StateMachine_Process_Stop_Loaded();
		printShablonRecepteLoaded((Recipe_t*)&current_recipe, &current_state);//Перезарядили шаблон и нецепт
//		currentProcessBuf = 
		deviceStatus.flagRegimOn = 0;
		ST7567_FB_fillRect(	label_ShablonState.posX, label_ShablonState.posY, 
											128-label_ShablonCurentTemperature.posX, 13, 0);
		label_ShablonState.text = stringPusk;//Меняем надпись
		//Возвращаем функцию Назад
		menu_RecepteLoadedTime.next = &menu_RecepteLoadedNazad;				
	//	menu_ShablonState.previous = &menu_ShablonSkip;
		menu_RecepteLoadedState.previous = &menu_RecepteLoadedNazad;		
		mainProcess = functionNull;//Отключаем функцию
		deviceStatus.pidEnable = 0;
		pid_Off();
		
		//Стираем название процесса
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
	}

	MENU_Init(&menu_RecepteLoadedState);
}

/*
Выводим на экран таблицу для отображеия хода автоматического процесса
чтобы не плодить буфера используем receptControl[0]
*/
void printShablonRecepteLoaded(Recipe_t *recept, SystemState_t* processState){
	allMenuTextClear();
//	ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
	ST7567_FB_printText(label_Ustavka.posX, label_Ustavka.posY, (char*)label_Ustavka.text, NORMAL);
	ST7567_FB_printText(label_Sostoianie.posX, label_Sostoianie.posY, (char*)label_Sostoianie.text, NORMAL);	
	ST7567_FB_printText(label_Temperature.posX, label_Temperature.posY, (char*)label_Temperature.text, NORMAL);		
	ST7567_FB_printText(label_Time.posX, label_Time.posY, (char*)label_Time.text, NORMAL);
	
	//Грузим установочные значения в массив
	receptControl[0].targetTimer_h = recept->mash_stages[current_mash_stage_Loaded].time / 60;
	receptControl[0].targetTimer_m = recept->mash_stages[current_mash_stage_Loaded].time % 60;
	receptControl[0].targetTemperature = recept->mash_stages[current_mash_stage_Loaded].temperature/10;
	
	//Выводим на экран начальное значение времени и температуры
	label_ShablonTemperature.text = getTemperatureStr(receptControl,0);//Работаем конкретно с ячейкой 0
	label_ShablonTimer.text =  getTimerStr(receptControl,0);
	MENU_Init(&menu_RecepteLoadedTempersture);//Инициализация
}


/*
Запускаем автоматический процесс
*/
void shablonSetFunctionAutoLoaded(void){

	if(!deviceStatus.flagRegimOn){
		deviceStatus.flagRegimOn = 1;
		mainProcess = StateMachine_Process;
		StateMachine_Process_Start();//Запустили процесс
	//	mainProcess = shablonExecute;//Процесс который будет запускаться в основном цикле
		label_ShablonState.text = stringStop;//Меняем надпись
		//Блокируем функции Назад и пропустить
		menu_RecepteLoadedTime.next = 			&menu_RecepteLoadedState;				
		menu_RecepteLoadedState.previous = 	&menu_RecepteLoadedTime;
		
		
		//Сотрем надпись Дальше и Назад
		ST7567_FB_fillRect(label_ShablonNazad.posX, label_ShablonNazad.posY, 90, 13, 0);	
		
		//пишем название процесса
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
		ST7567_FB_printText(1, 1, (char*)_stringMesh, NORMAL);
		//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = current_recipe.mash_stages[current_mash_stage_Loaded].time / 60;
		receptControl[0].targetTimer_m = current_recipe.mash_stages[current_mash_stage_Loaded].time % 60;
		receptControl[0].targetTemperature = current_recipe.mash_stages[current_mash_stage_Loaded].temperature/10;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
		deviceStatus.pidEnable = 1;
	}
	else{
		StateMachine_Process_Stop();
		printShablonRecepteLoaded((Recipe_t*)&current_recipe, &current_state);//Перезарядили шаблон и нецепт
//		currentProcessBuf = 
		deviceStatus.flagRegimOn = 0;
		ST7567_FB_fillRect(	label_ShablonState.posX, label_ShablonState.posY, 
											128-label_ShablonCurentTemperature.posX, 13, 0);
		label_ShablonState.text = stringPusk;//Меняем надпись
		//Возвращаем функцию Назад
		menu_RecepteLoadedTime.next = &menu_RecepteLoadedNazad;				
	//	menu_ShablonState.previous = &menu_ShablonSkip;
		menu_RecepteLoadedState.previous = &menu_RecepteLoadedNazad;		
		mainProcess = functionNull;//Отключаем функцию
		deviceStatus.pidEnable = 0;
		pid_Off();
		
		//Стираем название процесса
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
	}

	MENU_Init(&menu_RecepteLoadedState);
}

/**/
void shablonSetFunctionAutoResnartLoaded(void){
	ST7567_FB_fillRect(1, 1, 48, 13, 0);
	ST7567_FB_printText(1, 1, (char*)_stringMesh, NORMAL);
//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = current_recipe.mash_stages[current_mash_stage_Loaded].time / 60;
		receptControl[0].targetTimer_m = current_recipe.mash_stages[current_mash_stage_Loaded].time % 60;
		receptControl[0].targetTemperature = current_recipe.mash_stages[current_mash_stage_Loaded].temperature/10;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
	label_ShablonTemperature.text = getTemperatureStr(receptControl,0);//Работаем с 0-й ячейкой массива
	label_ShablonTimer.text =  getTimerStr(receptControl,0);
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	MENU_Init(&menu_RecepteLoadedState);
}
/**/
void shablonSetFunctionFilnrationAutoResnartLoaded(void){
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
		ST7567_FB_printText(1, 1, (char*)_stringFiltracion, NORMAL);
//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = 0;//Жестко 30 минут
		receptControl[0].targetTimer_m = 30;
		receptControl[0].targetTemperature = 77;//Всегда одна
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		label_ShablonTemperature.text = getTemperatureStr(receptControl,0);
	label_ShablonTimer.text =  getTimerStr(receptControl,0);
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	MENU_Init(&menu_RecepteLoadedState);
}

/**/
void shablonSetFunctionBoilingAutoResnartLoaded(void){
	//Пишем название процесса
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
	ST7567_FB_printText(1, 1, (char*)_stringNagrev, NORMAL);
//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = current_recipe.boil_time / 60;
		receptControl[0].targetTimer_m = current_recipe.boil_time % 60;
		receptControl[0].targetTemperature = 98;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		label_ShablonTemperature.text = getTemperatureStr(receptControl,0);
		label_ShablonTimer.text =  getTimerStr(receptControl,0);
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	MENU_Init(&menu_RecepteLoadedState);
}


/**/
void shablonSetFunctionCoolingAutoResnartLoaded(void){
		ST7567_FB_fillRect(1, 1, 48, 13, 0);
	ST7567_FB_printText(1, 1, (char*)_stringСold, NORMAL);
//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = 20;
		receptControl[0].targetTimer_m = 0;
		receptControl[0].targetTemperature = current_recipe.fermentation_temp/10;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		label_ShablonTemperature.text = getTemperatureStr(receptControl,0);
		label_ShablonTimer.text =  getTimerStr(receptControl,0);
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	MENU_Init(&menu_RecepteLoadedState);
}

/**/
void shablonSetFunctionFermentingAutoResnartLoaded(void){
			ST7567_FB_fillRect(1, 1, 48, 13, 0);
	ST7567_FB_printText(1, 1, (char*)_stringFermentation, NORMAL);
//Устанавливаем настройки температуры и времени для текущего цикла
		receptControl[0].targetTimer_h = current_recipe.fermentation_days * 24;
		receptControl[0].targetTimer_m = 0;
		receptControl[0].targetTemperature = current_recipe.fermentation_temp/10;
		deviceStatus.manualControlCurrentData.targetTemperature = receptControl[0].targetTemperature;
		
		//Копируем установки таймера в рабочую структуру
		deviceStatus.manualControlCurrentData.targetTimer_h =  receptControl[0].targetTimer_h;//Дублируем пересчет
		deviceStatus.manualControlCurrentData.targetTimer_m =  receptControl[0].targetTimer_m; 
		deviceStatus.manualControlCurrentData.targetTimer_s = 0;
		label_ShablonTemperature.text = getTemperatureStr(receptControl,0);
		label_ShablonTimer.text =  getTimerStr(receptControl,0);
		pid_set_setpoint(&pid, deviceStatus.manualControlCurrentData.targetTemperature);
	MENU_Init(&menu_RecepteLoadedState);
}

/**/
void stopProcessLoaded(void){
	ST7567_FB_fillRect(1, 1, 48, 13, 0);
							deviceStatus.flagRegimOn = 0;
							ST7567_FB_fillRect(	label_ShablonState.posX, label_ShablonState.posY, 
																128-label_ShablonCurentTemperature.posX, 13, 0);
							label_ShablonState.text = stringPusk;//Меняем надпись
							//Возвращаем функцию Назад
							menu_RecepteLoadedTime.next = &menu_RecepteLoadedNazad;				
							//menu_ShablonState.previous = &menu_ShablonSkip;
							menu_RecepteLoadedState.previous = &menu_RecepteLoadedNazad;	
							mainProcess = functionNull;//Отключаем функцию
							deviceStatus.pidEnable = 0;
							pid_Off();
	MENU_Init(&menu_RecepteLoadedState);
}

