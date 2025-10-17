#include "button.h"
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "delay.h"
#include "MPR121.h"

//static uint64_t timerButtonRead = 0;
//const uint64_t buttonReadPeriod = 40;//время между опросом кнопок, мс
//static uint16_t buttonArray[16];//Собираем сюда прочитанное состояние кнопок
//static uint8_t buttonArrayCnt = 0;
//uint16_t allButtonsRAW = 0;
//uint16_t allButtonsRAW_old;
///*
//Опрашивать в самом быстром цикле!!!
//Читаем состояние кнопок и если 
//*/
//uint16_t buttonRead(void){
//	if(millis() - timerButtonRead > buttonReadPeriod){
//		timerButtonRead = millis();
//		buttonArray[buttonArrayCnt] = mpr121_get_touch(MPR121_ADDRESS_BUTTONS);	
//		buttonArrayCnt++;
//		buttonArrayCnt %= 16;
//		allButtonsRAW = buttonArray[0] & buttonArray[1] & 
//								buttonArray[2] & buttonArray[3] & 
//								buttonArray[4] & buttonArray[5] & 
//								buttonArray[6] & buttonArray[7] &
//								buttonArray[8] & buttonArray[9] &
//								buttonArray[10] & buttonArray[11] &
//								buttonArray[12] & buttonArray[13] &
//								buttonArray[14] & buttonArray[15];
//		
//			
////		if(!!allButtonsRAW & (allButtonsRAW != allButtonsRAW_old)){
////				buzerEn = 1;
////				delay_ms(200);
////				buzerEn = 0;		
////		}
//		allButtonsRAW_old = allButtonsRAW;
//		
//	}
//	return allButtonsRAW;

//}
