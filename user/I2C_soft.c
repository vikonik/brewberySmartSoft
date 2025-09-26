#include "I2C_soft.h"
#include "MDR32_inout.h"
/*..............................................................................
Инициализация интерфейса I2C
..............................................................................*/
void I2Csft_Init(xI2C* i2cX) 
{ 
	PIO_Init_Pin(i2cX->port, i2cX->pin_scl, Output_Open_Drain_Up);
	PIO_Init_Pin(i2cX->port, i2cX->pin_sda, Output_Open_Drain_Up);
}

/*..............................................................................
Отправка одного байта в регистр по адресу выбранного I2C устройства
..............................................................................*/
void I2Csft_SendAdrReg(xI2C* i2cX, uint8_t adr, uint8_t reg, uint8_t val) 
{
  I2Csft_Start(i2cX);
  I2Csft_SendByte(i2cX, (adr<<1)|0);
  I2Csft_SendByte(i2cX, reg);
  I2Csft_SendByte(i2cX, val);
  I2Csft_Stop(i2cX);
}

/*..............................................................................
Генерация "СТАРТ"
..............................................................................*/
void I2Csft_Start(xI2C* i2cX) 
{ 
	PIO_SetBits(i2cX->port, i2cX->pin_scl); 		 
  PIO_SetBits(i2cX->port, i2cX->pin_sda); 		
	I2Csft_Wait(i2cX->tim);
	 
  PIO_ResetBits(i2cX->port, i2cX->pin_sda); 	I2Csft_Wait(i2cX->tim);
  PIO_ResetBits(i2cX->port, i2cX->pin_scl); 	 
}
/*..............................................................................
Генерация "СТОП"
..............................................................................*/
void I2Csft_Stop(xI2C* i2cX) 
{  
  PIO_ResetBits(i2cX->port, i2cX->pin_sda); 	I2Csft_Wait(i2cX->tim);
  PIO_SetBits(i2cX->port, i2cX->pin_scl);  		I2Csft_Wait(i2cX->tim); 
  PIO_SetBits(i2cX->port, i2cX->pin_sda);  		I2Csft_Wait(i2cX->tim);  
}

/*..............................................................................
Задержка для тактирования
..............................................................................*/
void I2Csft_Wait(uint16_t timing) 
{
  while(timing != 0)
    timing--;
}

/*..............................................................................
Отправка байта по выбранному I2C
..............................................................................*/
bool I2Csft_SendByte(xI2C* i2cX, uint8_t data) 
{  
  for(uint8_t i = 0; i < 8; i++) {
    if (data & 0x80) {
      PIO_SetBits(i2cX->port, i2cX->pin_sda);
    }
    else {
      PIO_ResetBits(i2cX->port, i2cX->pin_sda);
    }
    I2Csft_Wait(i2cX->tim);
    PIO_SetBits(i2cX->port, i2cX->pin_scl);
    data<<=1;
    I2Csft_Wait(i2cX->tim);
    PIO_ResetBits(i2cX->port, i2cX->pin_scl); 
  }   
	I2Csft_Wait(i2cX->tim); 																						// В этот момент ведомый генерит ACK  
	PIO_Init_Pin(i2cX->port, i2cX->pin_sda, Input_Pull_Up);
	PIO_SetBits(i2cX->port, i2cX->pin_scl);
  I2Csft_Wait(i2cX->tim); 																						// В этот момент ведомый передаёт ACK
	bool ack = PIO_ReadInputDataBit(i2cX->port, i2cX->pin_sda);					// Считать ACK 
  PIO_ResetBits(i2cX->port, i2cX->pin_scl);														// приём ACK завершён
	PIO_Init_Pin(i2cX->port, i2cX->pin_sda, Output_Open_Drain_Up);
	I2Csft_Wait(i2cX->tim);
 
  return ack;																													// вернуть ACK (0) или NACK (1) - ошибка
}

/*..............................................................................
Чтение одного байта с выбранного I2C по адресу устройства с регистра
..............................................................................*/
uint8_t I2Csft_GetAdrReg(xI2C* i2cX, uint8_t adr, uint8_t reg) 
{ 
	I2Csft_Start(i2cX);
	I2Csft_SendByte(i2cX, (adr<<1)|0);
  I2Csft_SendByte(i2cX, reg);
	I2Csft_Restart(i2cX);  
	I2Csft_SendByte(i2cX, (adr<<1)|1);
	uint8_t data = I2Csft_ReadByte(i2cX);
  I2Csft_Stop(i2cX); 
  return data;
}

/*..............................................................................
Генерация "РЕСТАРТ"
..............................................................................*/
void I2Csft_Restart(xI2C* i2cX) 
{  
	PIO_ResetBits(i2cX->port, i2cX->pin_scl); 	I2Csft_Wait(i2cX->tim);
	PIO_SetBits(i2cX->port, i2cX->pin_sda); 	  I2Csft_Wait(i2cX->tim);
	PIO_SetBits(i2cX->port, i2cX->pin_scl); 		I2Csft_Wait(i2cX->tim); 
	PIO_ResetBits(i2cX->port, i2cX->pin_sda); 	I2Csft_Wait(i2cX->tim);
	PIO_ResetBits(i2cX->port, i2cX->pin_scl); 	I2Csft_Wait(i2cX->tim);
}

/*..............................................................................
Чтение байта с выбранного I2C
..............................................................................*/
uint8_t I2Csft_ReadByte(xI2C* i2cX) 
{ 
  uint8_t byte;  
	PIO_Init_Pin(i2cX->port, i2cX->pin_sda, Input_Pull_Up);
  for(uint8_t i = 0; i < 8; i++) { 
		byte<<=1;
    PIO_SetBits(i2cX->port, i2cX->pin_scl);
    I2Csft_Wait(i2cX->tim); 
		if (PIO_ReadInputDataBit(i2cX->port, i2cX->pin_sda)) byte|=0x01; 
		PIO_ResetBits(i2cX->port, i2cX->pin_scl);
		I2Csft_Wait(i2cX->tim); 
  }
	bool ack = PIO_ReadInputDataBit(i2cX->port, i2cX->pin_sda); // Считать ACK
	PIO_Init_Pin(i2cX->port, i2cX->pin_sda, Output_Open_Drain_Up);
	if (ack) {
		PIO_SetBits(i2cX->port, i2cX->pin_sda);		// Без подтверждения, NACK, это последний считанный байт
	}
	else {
		PIO_ResetBits(i2cX->port, i2cX->pin_sda);	// Подтверждение, ACK, будем считывать ещё один байт
	}      
  I2Csft_Wait(i2cX->tim);
  PIO_SetBits(i2cX->port, i2cX->pin_scl);
  I2Csft_Wait(i2cX->tim);
  PIO_ResetBits(i2cX->port, i2cX->pin_scl);
	I2Csft_Wait(i2cX->tim);   
  PIO_SetBits(i2cX->port, i2cX->pin_sda);

  return byte;
}

xI2C I2Csft_Settings(void) 
{
	xI2C i2c;
	i2c.port = PORT_I2C_SOFT;
	i2c.pin_scl = PIN_I2C_SOFT_SCL;
	i2c.pin_sda = PIN_I2C_SOFT_SDA;
	i2c.tim = 0x01;			//0xFF;	//TBD выяснить за что отвечает этот параметр

	return i2c;
}

