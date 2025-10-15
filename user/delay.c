#include "delay.h"
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup



uint32_t tickDelay = 0;
uint64_t sysTickCount = 0;//Глобальная переменная времени
uint64_t sysTickGlobalTime = 0;
void delay_ms(uint32_t _tickDelay){
	tickDelay = _tickDelay; //* 1000;
  while(tickDelay);
}

uint64_t getTick(void){

	return sysTickCount;
}

/*
Аналог Ардуиновской функции
*/
uint64_t millis(void){

	return sysTickCount;
}

/**/
uint64_t getGlobalTime(void){
	return sysTickGlobalTime;
}

/**/
void setGlobalTime(uint64_t globalTime){
	sysTickGlobalTime = globalTime;
}
