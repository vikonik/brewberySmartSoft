#ifndef _DELAY_H
#define _DELAY_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup



void delay_ms(uint32_t _tickDelay);
    uint64_t getTick(void);
    uint64_t millis(void);
		uint64_t getGlobalTime(void);
		void setGlobalTime(uint64_t globalTime);
#endif
