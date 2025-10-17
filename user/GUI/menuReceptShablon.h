#ifndef _MENU_RECEPT_SHABLON_H
#define _MENU_RECEPT_SHABLON_H
#include "menu.h"
#include "allDefenition.h"
#include "recept.h"
#include "state_machine.h"

extern menu_t menu_ShablonTempersture, menu_ShablonTime, menu_ShablonNazad,menu_ShablonSkip, menu_ShablonState;
extern MenuText_t label_ShablonTemperature, label_ShablonTimer;

void printShablon(ShablonlControl_t* control, menu_t *menuTemperature,  menu_t *menuTime, MenuText_t* labelTemperatire, MenuText_t* labelTime);
void printShablonAuto(Recipe_t *recept, SystemState_t* processState);

void shablonSetFunctionAuto(void);
void shablonPrintCurrentData(void);
void shablonSetFunctionAutoResnart(void);
void shablonSetFunctionBoilingAutoResnart(void);
void shablonSetFunctionCoolingAutoResnart(void);
void shablonSetFunctionFermentingAutoResnart(void);
void shablonSetFunctionFilnrationAutoResnart(void);
void stopProcess(void);
#endif

