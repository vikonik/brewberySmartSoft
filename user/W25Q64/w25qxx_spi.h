#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H
#include "MDR32FxQI_ssp.h" 


void w25qxx_spi_Init(void);
uint8_t SPI_transmit(MDR_SSP_TypeDef* SPIx,uint8_t data);
#endif
