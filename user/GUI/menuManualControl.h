#ifndef _MANUSL_CONTROL_H
#define _MANUSL_CONTROL_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "allDefenition.h"

void printTable(void);
void printMenuManualControl(void);
char* getTemperatureStr(void);
char* getTimerStr(void);
void changeValue(uint16_t* _allButtonsRAW, ShablonlControl_t *manualControl);
void changeValueTime(uint16_t* _allButtonsRAW, ShablonlControl_t *manualControl);
void setButtonNavigationToChangeValueTemp(void);
void setButtonNavigationToChangeValueTimer(void);
void setMenuNavigationToChangeValue(void);
void shablonSetFunction(void);
void shablonExecute(void);
void distanceSetTemperature(float newTemperature, void(*func)(void));
void distanceSetTimer(uint16_t newTimer, void(*func)(void));
void printShablonManalControl(void);
#endif

