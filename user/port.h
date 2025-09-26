#ifndef PORT_H
#define PORT_H
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT


void initPort(  MDR_PORT_TypeDef*       MDR_PORTx, 
                PORT_Pin_TypeDef        PORT_Pin,
                PORT_OE_TypeDef         PORT_OE,       
                PORT_PULL_UP_TypeDef    PORT_PULL_UP,   
                PORT_PULL_DOWN_TypeDef  PORT_PULL_DOWN,
                PORT_PD_SHM_TypeDef     PORT_PD_SHM,    
                PORT_PD_TypeDef         PORT_PD,       
                PORT_GFEN_TypeDef       PORT_GFEN,     
                PORT_FUNC_TypeDef       PORT_FUNC,     
                PORT_SPEED_TypeDef      PORT_SPEED,     
                PORT_MODE_TypeDef       PORT_MODE);
#endif
