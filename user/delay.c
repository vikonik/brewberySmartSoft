#include "delay.h"
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup



uint32_t tickDelay = 0;
uint64_t sysTickCount = 0;//Глобальная переменная времени

void delay_ms(uint32_t _tickDelay){
	tickDelay = _tickDelay; //* 1000;
  while(tickDelay);
}

uint32_t getTick(void){

	return sysTickCount;
}

/*
Аналог Ардуиновской функции
*/
uint32_t millis(void){

	return sysTickCount;
}
