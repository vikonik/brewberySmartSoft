#ifndef _MANIFEST_H
#define _MANIFEST_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup


#define charHeight8pt 12

extern const char stringDalshe[];
extern const char stringNazad[];
extern const char stringProdolgit[];
extern const char stringOtmena[];
extern const char stringOK[];
extern const char stringSkip[];
extern const char stringBegin[];	

extern const char stringRecept[];
extern const char stringClearing[] ;
extern const char stringManualControl[];
extern const char stringSetting[] ;
/******* Сорта пива ************/
extern const char stringPsenichnoe[];
extern const char stringTemnoe[];
extern const char stringSvetloe[];

/*********** Очистка ****************/
extern const char stringLowTemp[];
extern const char stringWoshingState_1[];
extern const char stringWoshing[];
extern const char stringWoshingState_2[];
extern const char stringWoshingRinsing[];
extern const char stringWoshingState_3[];
extern const char stringWoshingDesinfektion[];
extern const char stringWoshingComplite[];

/************ Рецепт ********************/
extern const char stringReceptIns[];
extern const char stringReceptLoaded[];
extern const char stringNoRecepte[];
extern const char stringReseptDell[];
	
/************* Ручное управление **************/
extern const char stringMesh[]; 
extern const char stringFermentation[]; 
extern const char stringFiltracion[]; 
extern const char stringNagrev[];
extern const char stringСold[];

extern const char _stringMesh[];// "Затирание"; 
extern const char _stringFermentation[];//"Ферментация"; 
extern const char _stringFiltracion[];// "Фильтрация"; 
extern const char _stringNagrev[];//"Нагрев";
extern const char _stringСold[]	;
	
/************* Шаблон для фильтрации и ферментации **************/
extern const char stringTemperature[]; 
extern const char stringTime[]; 
extern const char stringUstavka[]; 
extern const char stringSostoianie[]; 
extern const char stringPusk[]; 
extern const char stringStop[];
/********************* Ошибка датчика ***********************/
extern const char tempSensorError[];

/********************* Меню Настройка ***********************/
extern const char stringBtOn[];
extern const char stringBtOff[];
extern const char stringWiFiOn[];
extern const char stringWiFiOff[];
extern const char stringMuteOn[];
extern const char stringMuteOff[];





#endif
