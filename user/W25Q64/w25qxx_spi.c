#include "w25qxx_spi.h"
#include "MDR32FxQI_ssp.h"              // Milandr::Drivers:SSP
#include "allDefenition.h"
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT

void w25qxx_spi_Init(void){
    SSP_InitTypeDef sSSP;
    PORT_InitTypeDef PORT_InitStructure;
    
    PORT_StructInit(&PORT_InitStructure);
		PORT_InitStructure.PORT_Pin   = (PIN_FLASH_CS);
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
		PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(PORT_FLASH, &PORT_InitStructure);
	

    PORT_InitStructure.PORT_Pin   = (PIN_FLASH_MISO );
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
    PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
		PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(PORT_FLASH, &PORT_InitStructure);

    PORT_InitStructure.PORT_Pin   = (PIN_FLASH_MOSI | PIN_FLASH_SCK );
    PORT_InitStructure.PORT_OE    = PORT_OE_IN;
    PORT_InitStructure.PORT_PULL_UP = PORT_PULL_UP_ON;
    PORT_InitStructure.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
		PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(PORT_FLASH, &PORT_InitStructure);
		
		
		//!!!Этот SPI используется дисплеем но на других выводах!!!!
	
//	/* Reset all SSP settings */
//    SSP_DeInit(SPI_FLASH);


//    SSP_BRGInit(SPI_FLASH, SSP_HCLKdiv2);


//    /* SSP1 MASTER configuration */
//    SSP_StructInit (&sSSP);

//    sSSP.SSP_SCR        = 0x10;
//    sSSP.SSP_CPSDVSR    = 2;
//    sSSP.SSP_Mode       = SSP_ModeMaster;
//    sSSP.SSP_WordLength = SSP_WordLength8b;
//    sSSP.SSP_SPH        = SSP_SPH_1Edge;
//    sSSP.SSP_SPO        = SSP_SPO_Low;//SSP_SPO_High
//    sSSP.SSP_FRF        = SSP_FRF_SPI_Motorola;
//    sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
//    SSP_Init (SPI_FLASH,&sSSP);
//		    /* Enable SSP1 */
//    SSP_Cmd(SPI_FLASH, ENABLE);   
//    
}

/**/
uint8_t SPI_transmit(MDR_SSP_TypeDef* SPIx,uint8_t data){//0-command 1-data
	SSP_SendData(SPIx,data);
	while(SSP_GetFlagStatus(SPIx, SSP_FLAG_BSY));
	return((uint8_t)SSP_ReceiveData(SPIx));
}
