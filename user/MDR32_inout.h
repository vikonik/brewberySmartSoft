#ifndef MDR32_inout_H
#define MDR32_inout_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT

typedef enum {											
	Output_Open_Drain_Up,	
	Input_Pull_Up,
	Input_Ext_Interrupt,
} Port_Config;

void PinInit_OUT(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin) ;
void PinInit_IN(MDR_PORT_TypeDef* PORTx, uint32_t PORT_Pin) ;

void PIO_SetBits(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin);
void PIO_ResetBits(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin);
uint8_t PIO_ReadInputDataBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin);

void PIO_Init_Pin(MDR_PORT_TypeDef* PORTx, uint16_t PORT_Pin, Port_Config config);
void PIO_ToogleBit(MDR_PORT_TypeDef* PORTx, PORT_Pin_TypeDef PORT_Pin);
#endif
