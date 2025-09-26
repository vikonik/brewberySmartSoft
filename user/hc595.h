#ifndef HC595_H
#define HC595_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup


  #define HC595_ON PORT_ResetBits(PORT_HC595_CS, PIN_HC595_CS);
  #define HC595_OFF  PORT_SetBits(PORT_HC595_CS, PIN_HC595_CS);
    
void hc595Init(void);
void  writnwReg(uint16_t v);
void HC595_Init(void);
void HC595_SendByte(uint16_t data);


#endif
