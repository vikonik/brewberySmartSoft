#ifndef __MPR121_H
#define __MPR121_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup


#include "stdbool.h" 
//#include "I2C_soft.h"

// Адрес
#define MPR121_ADDRESS_BUTTONS		0x5A
#define MPR121_ADDRESS_SLIDER			0x5B

#define NUM_OF_ELECTRODES			13

// Статусы OOR и прикосновений
#define TS1										0x00 
#define TS2 									0x01 
#define OORS1 								0x02 
#define OORS2 								0x03 

// Фильтр
#define E0FDL 								0x04 
#define E0FDH 								0x05 
#define E1FDL 								0x06 
#define E1FDH 								0x07 
#define E2FDL 								0x08 
#define E2FDH 								0x09 
#define E3FDL 								0x0A 
#define E3FDH 								0x0B 
#define E4FDL 								0x0C 
#define E4FDH 								0x0D 
#define E5FDL 								0x0E 
#define E5FDH 								0x0F 
#define E6FDL 								0x10 
#define E6FDH 								0x11 
#define E7FDL 								0x12 
#define E7FDH 								0x13 
#define E8FDL 								0x14 
#define E8FDH 								0x15 
#define E9FDL 								0x16 
#define E9FDH 								0x17 
#define E10FDL 								0x18 
#define E10FDH 								0x19 
#define E11FDL 								0x1A 
#define E11FDH 								0x1B 
#define E12FDL 								0x1C 
#define E12FDH 								0x1D 

// Базовые значения
#define E0BV 									0x1E 
#define E1BV 									0x1F 
#define E2BV 									0x20 
#define E3BV 									0x21 
#define E4BV 									0x22 
#define E5BV 									0x23 
#define E6BV 									0x24 
#define E7BV 									0x25 
#define E8BV 									0x26 
#define E9BV 									0x27 
#define E10BV 								0x28 
#define E11BV 								0x29 
#define E12BV 								0x2A 

// Фильтр восхождения сигнала
#define MHDR 									0x2B 
#define NHDR 									0x2C 
#define NCLR 									0x2D 
#define FDLR 									0x2E 

#define MHDPROXR 							0x36 
#define NHDPROXR 							0x37 
#define NCLPROXR 							0x38 
#define FDLPROXR 							0x39 

// Фильтр спада сигнала
#define MHDF 									0x2F 
#define NHDF 									0x30 
#define NCLF 									0x31 
#define FDLF 									0x32 

#define MHDPROXF 							0x3A 
#define NHDPROXF							0x3B 
#define NCLPROXF 							0x3C 
#define FDLPROXF 							0x3D 

// Фильтр касания
#define NHDT 									0x33 
#define NCLT 									0x34 
#define FDLT 									0x35 

#define NHDPROXT 							0x3E 
#define NCLPROXT 							0x3F 
#define FDLPROXT 							0x40

// Пороги страбатывания 
#define E0TTH 								0x41 
#define E0RTH 								0x42 
#define E1TTH 								0x43 
#define E1RTH 								0x44 
#define E2TTH 								0x45 
#define E2RTH 								0x46 
#define E3TTH 								0x47 
#define E3RTH 								0x48 
#define E4TTH 								0x49 
#define E4RTH 								0x4A 
#define E5TTH 								0x4B 
#define E5RTH 								0x4C 
#define E6TTH 								0x4D 
#define E6RTH 								0x4E 
#define E7TTH 								0x4F 
#define E7RTH 								0x50 
#define E8TTH 								0x51 
#define E8RTH 								0x52 
#define E9TTH 								0x53 
#define E9RTH 								0x54 
#define E10TTH 								0x55 
#define E10RTH 								0x56 
#define E11TTH 								0x57 
#define E11RTH 								0x58 
#define E12TTH 								0x59 
#define E12RTH 								0x5A 

// Дребезг контактов
#define DTR 									0x5B 

// Регистры конфигурации
#define AFE1 									0x5C 
#define AFE2 									0x5D 
#define ECR 									0x5E 

/// Токи
#define CDC0									0x5F
#define CDC1									0x60
#define CDC2									0x61
#define CDC3									0x62
#define CDC4									0x63
#define CDC5									0x64
#define CDC6									0x65
#define CDC7									0x66
#define CDC8									0x67
#define CDC9									0x68
#define CDC10 								0x69
#define CDC11 								0x6A
#define CDC12 								0x6B

// Время заряда
#define CDT01 								0x6C
#define CDT23 								0x6D
#define CDT45 								0x6E
#define CDT67 								0x6F
#define CDT89 								0x70
#define CDT1011 							0x71
#define CDT11 								0x72

// GPIO
#define CTL0									0x73
#define CTL1 									0x74
#define DAT 									0x75
#define DIR 									0x76
#define EN 		    						0x77
#define SET 									0x78
#define CLR 									0x79
#define TOG 									0x7A

// Автоконфигурация
#define ACCR0									0x7B
#define ACCR1									0x7C
#define USL		    						0x7D
#define LSL		    						0x7E
#define TL		    						0x7F

// Программный сброс
#define SRST									0x80

// PWM
#define PWM0									0x81
#define PWM1									0x82
#define PWM2									0x83
#define PWM3 									0x84

// Глобальные константы 				
#define TOU_THRESH						0x14 //0x14			//Для увеличения чувствительности нажатия уменьшить это значение, 		0x08 - протестировано с черным стеклом
#define	REL_THRESH						0x04 //0x08			//Для увеличения чувствительности отпускания уменьшить это значение, 	0x04 - протестировано с черным стеклом

bool mpr121_init(uint8_t i2c_addr);
void write_register(uint8_t i2c_addr, uint8_t reg_addr, uint8_t value);
void read_register(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *value_ptr);
uint16_t mpr121_get_touch(uint8_t i2c_addr);

#endif /* __MPR121_H */
