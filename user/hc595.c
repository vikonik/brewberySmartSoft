#include "hc595.h"
#include "allDefenition.h"
#include "MDR32FxQI_ssp.h"  
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK

#include "delay.h"
uint8_t hc595NumOfReg = 1;
/**/
void hc595Init(void){
		SSP_InitTypeDef sSSP;
		PORT_InitTypeDef PORT_InitStructure;

		PORT_StructInit(&PORT_InitStructure);
		PORT_InitStructure.PORT_Pin   = (PIN_HC595_CS);
		PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
		PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
		PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
		PORT_Init(PORT_HC595_CS, &PORT_InitStructure);


		PORT_InitStructure.PORT_Pin   = (PIN_HC595_MOSI | PIN_HC595_CSK );
		PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
		PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_OFF;
		PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_ON;
		PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
		PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
		PORT_Init(PORT_HC595_CS, &PORT_InitStructure);

	
	/* Reset all SSP settings */
    SSP_DeInit(MDR_SSP2);


    SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv128);


    /* SSP1 MASTER configuration */
    SSP_StructInit (&sSSP);

    sSSP.SSP_SCR        = 0x10;
    sSSP.SSP_CPSDVSR    = 2;
    sSSP.SSP_Mode       = SSP_ModeMaster;
    sSSP.SSP_WordLength = SSP_WordLength8b;
    sSSP.SSP_SPH        = SSP_SPH_2Edge;
    sSSP.SSP_SPO        = SSP_SPO_Low;
    sSSP.SSP_FRF        = SSP_FRF_SPI_Motorola;
    sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
    SSP_Init (MDR_SSP2,&sSSP);
		    /* Enable SSP1 */
    SSP_Cmd(MDR_SSP2, ENABLE);   
    
}

/**/
void  writnwReg(uint16_t v){
 //   PORT_ResetBits(PORT_HC595_CS, PIN_HC595_CS);
	/* loop while data register in not emplty */
	while (RESET == SSP_GetFlagStatus(MDR_SSP2,SSP_FLAG_TFE)  );
	/* send byte through the SPI0 peripheral */
	SSP_SendData(MDR_SSP2, v);   
    
//	/* wait to receive a byte */
	while(RESET == SSP_GetFlagStatus(MDR_SSP2,SSP_FLAG_RNE));
//	/* return the byte read from the SPI bus */
//	return SSP_ReceiveData(MDR_SSP1);
    delay_ms(1);
    PORT_ResetBits(PORT_HC595_CS, PIN_HC595_CS);
    delay_ms(1);
    PORT_SetBits(PORT_HC595_CS, PIN_HC595_CS);
}

/**/
void HC595_Init(uint8_t numOfReg) {
	
    PORT_InitTypeDef PORT_InitStructure;

    PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = PIN_HC595_MOSI | PIN_HC595_CSK ;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(PORT_HC595_SPI, &PORT_InitStructure);
    
    PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = PIN_HC595_CS;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(PORT_HC595_CS, &PORT_InitStructure);
    
    
    // 3. Установить начальное состояние пинов (обычно низкое).
    PORT_ResetBits(PORT_HC595_CS,PIN_HC595_MOSI);
    PORT_ResetBits(PORT_HC595_CS,PIN_HC595_CSK);
    PORT_ResetBits(PORT_HC595_CS,PIN_HC595_CS);
		
		hc595NumOfReg = numOfReg;
}


/*
Аргументом передаем указатель на начало пересылаемых данных.
Количество байт должно быть не меньше чем количество регистров hc595NumOfReg. 
*/
void HC595_SendByte(uint8_t *data) {
    // 1. Активировать Chip Select (CS) - установить в низкий уровень.
    PORT_ResetBits(PORT_HC595_CS, PIN_HC595_CS);

    // 2. Передать каждый бит данных последовательно.  Начиная с *старшего* бита (MSB).
	for(uint8_t j = 0; j < hc595NumOfReg; j++){
    for (int i = 7; i >= 0; i--) {
        // Определить значение бита, который нужно передать.
        if ((*data >> i) & 0x01) {
            PORT_SetBits(PORT_HC595_SPI, PIN_HC595_MOSI); // Установить MOSI High
        } else {
            PORT_ResetBits(PORT_HC595_SPI, PIN_HC595_MOSI); // Установить MOSI Low
        }

        // Импульс тактового сигнала SCK (Serial Clock) - Clock signal must be stable
        // before and after HIGH pulse.
        PORT_ResetBits(PORT_HC595_SPI, PIN_HC595_CSK); // SCK Low
        for(volatile int i = 0; i < 10; i++); // Очень короткая задержка (настройте под вашу частоту)
        PORT_SetBits(PORT_HC595_SPI, PIN_HC595_CSK);    // SCK High
        for(volatile int i = 0; i < 10; i++); // Очень короткая задержка (настройте под вашу частоту)
    }
		data++;
	}
    // 3. Деактивировать Chip Select (CS) - установить в высокий уровень.
    PORT_SetBits(PORT_HC595_CS, PIN_HC595_CS);

    // 4. Поместить данные в выход. В данном случае не требуется отдельного пина для переброса данных в регистр вывода.
    //   Регистр HC595 самостоятельно делает это при записи 8 бит (в этом режиме работы).

}

