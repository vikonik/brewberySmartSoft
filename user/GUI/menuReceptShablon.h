#ifndef _MENU_RECEPT_SHABLON_H
#define _MENU_RECEPT_SHABLON_H
#include "menu.h"
#include "allDefenition.h"
#include "recept.h"
#include "state_machine.h"

extern menu_t menu_ShablonTempersture, menu_ShablonTime, menu_ShablonNazad,menu_ShablonSkip, menu_ShablonState;
extern MenuText_t label_ShablonTemperature, label_ShablonTimer;

char* getTemperatureStr(ShablonlControl_t* control, uint8_t addr ) ;
char* getTimerStr(ShablonlControl_t* control, uint8_t addr);
	
//void printShablon(ShablonlControl_t* control, menu_t *menuTemperature,  menu_t *menuTime, MenuText_t* labelTemperatire, MenuText_t* labelTime);
void printShablon(ShablonlControl_t* control, menu_t *menuTemperature,  menu_t *menuTime);
void printShablonAuto(Recipe_t *recept, SystemState_t* processState);

void changeValue(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data);
void changeValueTime(uint16_t* _allButtonsRAW, ShablonlControl_t *control, uint8_t data);

void shablonSetFunctionAuto(void);
void shablonPrintCurrentData(void);
void shablonSetFunctionAutoResnart(void);
void shablonSetFunctionBoilingAutoResnart(void);
void shablonSetFunctionCoolingAutoResnart(void);
void shablonSetFunctionFermentingAutoResnart(void);
void shablonSetFunctionFilnrationAutoResnart(void);
void stopProcess(void);
void setButtonNavigationToChangeValueTemp(void);
void setButtonNavigationToChangeValueTimer(void);
#endif

