#ifndef __I2C_SOFT_H
#define __I2C_SOFT_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

#include "allDefenition.h"


#include <stdbool.h>
//#include "common.h"
//#include "hardlayer.h"

void I2Csft_Init(xI2C* i2cX);
void I2Csft_SendAdrReg(xI2C* i2cX, uint8_t adr, uint8_t reg, uint8_t val);
void I2Csft_Start(xI2C* i2cX);
void I2Csft_Stop(xI2C* i2cX);
void I2Csft_Wait(uint16_t timing);
bool I2Csft_SendByte(xI2C* i2cX, uint8_t data);
uint8_t I2Csft_GetAdrReg(xI2C* i2cX, uint8_t adr, uint8_t reg);
void I2Csft_Restart(xI2C* i2cX);
uint8_t I2Csft_ReadByte(xI2C* i2cX);
xI2C I2Csft_Settings(void);

#endif /*__I2C_SOFT_H*/
