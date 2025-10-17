#include "MPR121.h" 
#include "delay.h" 
#include "allDefenition.h"
#include "MDR32FxQI_i2c.h"              // Milandr::Drivers:I2C
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "I2C_soft.h"
#include "MDR32_inout.h"


extern xI2C I2C_b;

bool mpr121_init(uint8_t i2c_addr) 
{
	bool success = true; 
	uint8_t electrodes_count;
	uint8_t reg_value = 0;  

	//PIO_Init_Pin(PORT_MPR_121, PIN_MPR_121_INT, Input_Ext_Interrupt);		//Инициализация входа контроллера как EXT_INT
	PinInit_IN(PORT_MPR_121, PIN_MPR_121_INT);		//Когда состояние кнопок изменяется на MPR121, то выставляется 0 и держится до тех пор, пока данные не вычитаны
	
	write_register(i2c_addr, SRST, 0x63); 				// Выполняем программный сброс
	delay_ms(1);
    //delay_us(150); 
	read_register(i2c_addr, AFE2, &reg_value); 		// Чтение AFE Configuration 2
	
	if (reg_value != 0x24) 							// Проверка значения по умолчанию
	{
		success = false;
	} 

	read_register(i2c_addr, TS2, &reg_value); 		// Читаем Touch Status register
	
	if ((reg_value & 0x80) != 0) 				// Проверка значения по умолчанию
	{  
		success = false;
	}  

	if (success == true) 								// Если всё OK
	{ 				
		write_register(i2c_addr, ECR, 0x00); 				// Отключаем все электроды

		// Запись регистров инициализации
		write_register(i2c_addr, MHDR, 0x02); //Увеличивает гистерезис - касание регистрируется позже
		write_register(i2c_addr, NHDR, 0x80);	//Увеличивает шумовой порог - меньше ложных касаний
		write_register(i2c_addr, NCLR, 0xF0); //Замедляет восстановление базы - меньше ложных срабатываний после касания			
		write_register(i2c_addr, FDLR, 0x0F); //Увеличивает задержку фильтра - плавнее реакция на касание			
		write_register(i2c_addr, MHDF, 0x02); //Увеличивает гистерезис - отпускание регистрируется позже     
		write_register(i2c_addr, NHDF, 0x02); //Увеличивает шумовой порог - меньше ложных отпусканий     
		write_register(i2c_addr, NCLF, 0xFF); //Замедляет реакцию на отпускание - предотвращает дрожание			
		write_register(i2c_addr, FDLF, 0xFF); //величивает задержку фильтра - плавнее реакция на отпускание			
		write_register(i2c_addr, NHDT, 0x08); //Увеличить: Больший гистерезис для определения касания/отпускания Уменьшить: Более чувствительное определение
		write_register(i2c_addr, NCLT, 0x0F); //Увеличить: Требуется больше отсчетов для подтверждения изменения состояния Уменьшить: Быстрее реагирует на изменения
		write_register(i2c_addr, FDLT, 0x0F); //Увеличить: Большая задержка фильтра - плавнее переходы Уменьшить: Меньшая задержка - быстрее реакция

//		write_register(MHDPROXR, 0x0F);
//		write_register(NHDPROXR, 0x0F);
//		write_register(NCLPROXR, 0x00);
//		write_register(FDLPROXR, 0x00);
//		write_register(MHDPROXF, 0x01);
//		write_register(NHDPROXF, 0x01);
//		write_register(NCLPROXF, 0xFF);
//		write_register(FDLPROXF, 0xFF);
//		write_register(NHDPROXT, 0x00);
//		write_register(NCLPROXT, 0x00);
//		write_register(FDLPROXT, 0x00);
		write_register(i2c_addr, DTR, 0x11);
		
//		write_register(i2c_addr, AFE1, 0xC9);  			// 34 выборки фильтра первого уровня, 9 μA
//		write_register(i2c_addr, AFE2, 0x90);	 			// 10 выборок фильтра второго уровня, время выборки 4 μs 
//		write_register(i2c_addr, AFE1, 0xCF);  			// Фильтр первого уровня. 34 выборки, ток 15 μA
//		write_register(i2c_addr, AFE2, 0x90);	 			// Фильтр второго уровня. Время зарядки/разрядки 2 мкс (ххх00000 * 0,5), 10 выборок фильтра второго уровня(000хх000), период 1 мс (2 в степени 00000ххх) 
//		write_register(i2c_addr, AFE1, 0x0F);  			// Фильтр первого уровня. 6 выборок, ток 15 μA
//		write_register(i2c_addr, AFE2, 0x90);	 			// Фильтр второго уровня. Время зарядки/разрядки 2 мкс (ххх00000 * 0,5), 10 выборок фильтра второго уровня(000хх000), период 1 мс (2 в степени 00000ххх) 

uint8_t _afei1 = (0<<6)|(0x0F);
uint8_t _afei2 = (2<<5)|(3 << 3)|(0x02);
write_register(i2c_addr, AFE1, _afei1); 
write_register(i2c_addr, AFE2, _afei2);			
		
		// Применяем настройки threshold для всех сенсоров
		for (electrodes_count = 0; electrodes_count < NUM_OF_ELECTRODES; electrodes_count++) 
		{
			write_register(i2c_addr, (E0TTH + (electrodes_count<<1)), TOU_THRESH); 
			write_register(i2c_addr, (E0RTH + (electrodes_count<<1)), REL_THRESH); 
		} 
			write_register(i2c_addr, (E0TTH + (10<<1)), TOU_THRESH + 2); 
			write_register(i2c_addr, (E0RTH + (10<<1)), REL_THRESH + 2); 
		
			write_register(i2c_addr, (E0TTH + (11<<1)), TOU_THRESH + 2); 
			write_register(i2c_addr, (E0RTH + (11<<1)), REL_THRESH + 2); 
		//Регистры автоконфигурации
//		write_register(i2c_addr, ACCR0, 0x00);
//		write_register(i2c_addr, ACCR1, 0x00);
//		write_register(i2c_addr, USL, 0x00); 
//		write_register(i2c_addr, LSL, 0x00); 
//		write_register(i2c_addr, TL, 0x00); 
		#define VDD  3.0
		write_register(i2c_addr, ACCR0, 0x0B);
		write_register(i2c_addr, ACCR1, 0x00);
		write_register(i2c_addr, USL, (VDD-0.7) / VDD * 256); 
		write_register(i2c_addr, LSL, (VDD-0.7) / VDD * 256 * 0.65); 
		write_register(i2c_addr, TL, (VDD-0.7) / VDD * 256 * 0.9); 
		//write_register(i2c_addr, TL, (VDD-0.7) / VDD * 256 * 0.95); 	//успешные тесты со стеклом проведены с этим значением, пока не удалять
		
		write_register(i2c_addr, ECR, 0xCC); 				// Включаем 12 электродов с запоминанием стартового состояния
	}  
	return success;
}

void write_register(uint8_t i2c_addr, uint8_t reg_addr, uint8_t value) 
{ 
	I2Csft_SendAdrReg(&I2C_b, i2c_addr, reg_addr, value);     
}

void read_register(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *value_ptr) 
{  
	*value_ptr = I2Csft_GetAdrReg(&I2C_b, i2c_addr, reg_addr);  
}  


uint16_t mpr121_get_touch(uint8_t i2c_addr) 
{
	uint8_t inp_l, inp_h = 0;
	uint16_t touch_flags = 0;  
	read_register(i2c_addr, TS1, &inp_l); 				// Считываем Touch 1 Status register  
	read_register(i2c_addr, TS2, &inp_h);					// Считываем Touch 2 Status register   
	touch_flags = BT16(inp_h, inp_l);
	return touch_flags;
}



