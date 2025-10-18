#include "anyTest.h"
//void pinTest(void){
//  RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTA | RST_CLK_PCLK_PORTC |RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF |RST_CLK_PCLK_EEPROM), ENABLE);

//initPort(DISPLAY_PORT,
//    PORT_Pin_2 |
//    PORT_Pin_3 |
//    PORT_Pin_4 |
//    PORT_Pin_5 |
//    PORT_Pin_6 |
//    PORT_Pin_7 |
//    
//    DISPLAY_D0 | 
//    DISPLAY_D1 | 
//    DISPLAY_D2 | 
//    DISPLAY_D3 | 
//    DISPLAY_WR | 
//    DISPLAY_nWR |
//    DISPLAY_AO ,
//    PORT_OE_OUT,
//    PORT_PULL_UP_OFF,
//    PORT_PULL_DOWN_OFF,
//    PORT_PD_SHM_OFF,
//    PORT_PD_DRIVER,
//    PORT_GFEN_OFF,
//    PORT_FUNC_PORT,
//    PORT_SPEED_MAXFAST,
//    PORT_MODE_DIGITAL );




//initPort(MDR_PORTA,
//    PORT_Pin_0 |
//    PORT_Pin_1 |
//    PORT_Pin_2  ,
//    PORT_OE_OUT,
//    PORT_PULL_UP_OFF,
//    PORT_PULL_DOWN_OFF,
//    PORT_PD_SHM_OFF,
//    PORT_PD_DRIVER,
//    PORT_GFEN_OFF,
//    PORT_FUNC_PORT,
//    PORT_SPEED_MAXFAST,
//    PORT_MODE_DIGITAL );



//initPort(MDR_PORTF,
//    PORT_Pin_5 |
//    PORT_Pin_6 |
//    PORT_Pin_7 |
//    PORT_Pin_8 |
//    PORT_Pin_9 |
//    PORT_Pin_10 |
//    PORT_Pin_11 |
//    PORT_Pin_13 |
//    PORT_Pin_15  ,
//    PORT_OE_OUT,
//    PORT_PULL_UP_OFF,
//    PORT_PULL_DOWN_OFF,
//    PORT_PD_SHM_OFF,
//    PORT_PD_DRIVER,
//    PORT_GFEN_OFF,
//    PORT_FUNC_PORT,
//    PORT_SPEED_MAXFAST,
//    PORT_MODE_DIGITAL );

//initPort(MDR_PORTC,
//    PORT_Pin_3 |
//    PORT_Pin_4 |
//    PORT_Pin_9 |
//    PORT_Pin_10 |
//    PORT_Pin_11 |
//    PORT_Pin_12 ,
//    PORT_OE_OUT,
//    PORT_PULL_UP_OFF,
//    PORT_PULL_DOWN_OFF,
//    PORT_PD_SHM_OFF,
//    PORT_PD_DRIVER,
//    PORT_GFEN_OFF,
//    PORT_FUNC_PORT,
//    PORT_SPEED_MAXFAST,
//    PORT_MODE_DIGITAL );
//    
//    while(1){
//    
//   delay_ms(500);
//    PORT_Write(DISPLAY_PORT, 0xFF);
//    PORT_Write(MDR_PORTA, 0xFF);
//    PORT_Write(MDR_PORTF, 0xFF);
//    PORT_Write(MDR_PORTC, 0xFF);    
//    
//     delay_ms(500);
//    PORT_Write(DISPLAY_PORT, 0x00);
//    PORT_Write(MDR_PORTA, 0x00);
//    PORT_Write(MDR_PORTF, 0x00);
//    PORT_Write(MDR_PORTC, 0x00);    
//     }
//    }

///**/
//void expressTest(void){
//	for(uint8_t i = 0; i < 10; i++){
//		for(uint8_t j = 0; j < 10; j++){
//			if(i == j){
//				displayArray[j] = digit[8];
//			}
//			else
//				displayArray[j] = digit[10];
//		}
//		
//	displayData(0, displayArray,10);
//	delay_ms(200);	
//	}
//	
//		for(uint8_t j = 0; j < 10; j++){
//			displayArray[j] = digit[SPASE];
//		}
//		
//	for(uint8_t i = 0; i < 10; i++){
//		
//		displayArray[i] = digit[DEFISE];

//	displayData(0, displayArray,10);
//	delay_ms(200);	
//	}
//	
////
//	delay_ms(500);
//	PORT_SetBits(RELAY_PORT,MIXER );		delay_ms(500);
//	PORT_SetBits(RELAY_PORT,HEAT_1 );		delay_ms(500);
//	PORT_SetBits(RELAY_PORT,HEAT_2 );		delay_ms(500);
//	PORT_ResetBits(RELAY_PORT,MIXER );	delay_ms(500);
//	PORT_ResetBits(RELAY_PORT,HEAT_1 );	delay_ms(500);
//	PORT_ResetBits(RELAY_PORT,HEAT_2 );	delay_ms(500);	
//	
//   buzerEn = 1;;
//    delay_ms(200);
//   buzerEn = 0;;

//	
//			for(uint8_t j = 0; j < 10; j++){

//				displayArray[j] = digit[SPASE];
//			}
//			displayData(0, displayArray,10);	
//			
//while(1){
//		if(PIO_ReadInputDataBit(PORT_MPR_121, PIN_MPR_121_INT) == 0)	//Когда контроллер сенсорных кнопок замечает изменение состояния кнопок, он выставляет линию в 0, пока данные не вычитаны
//		{
//			//_delay_ms(10);
//			allButtonsRAW = mpr121_get_touch(MPR121_ADDRESS_BUTTONS);		
//			
//			//hotpanel.allButtons = ReorderButtons(allButtonsRAW);				//Перегруппировка кнопок			
//			buzerEn = 1;
//			delay_ms(200);
//			buzerEn = 0;;
//		}	
//						
//		switch(allButtonsRAW){
//			case 0x0000:
//					displayArray[0] = digit[0];
//					displayData(0, displayArray,10);	
//				break;			
//			
//			case 0x0800:
//					displayArray[0] = digit[1];
//					displayData(0, displayArray,10);	
//				break;
//			
//			case 0x0400:
//					displayArray[0] = digit[2];
//					displayData(0, displayArray,10);	
//				break;	
//			
//			case 0x0200:
//					displayArray[0] = digit[3];
//					displayData(0, displayArray,10);	
//				break;	

//			case 0x0080:
//					displayArray[0] = digit[4];
//					displayData(0, displayArray,10);	
//				break;	
//			
//			case 0x0040:
//					displayArray[0] = digit[5];
//					displayData(0, displayArray,10);	
//				break;		
//			case 0x0020:
//					displayArray[0] = digit[6];
//					displayData(0, displayArray,10);	
//				break;			
//			default: break;
//		}

//}
//}		
