#ifndef _DELAY_H
#define _DELAY_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup



void delay_ms(uint32_t _tickDelay);
    uint32_t getTick(void);
    uint32_t millis(void);
#endif
