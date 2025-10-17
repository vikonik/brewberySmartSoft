#include "flashController.h"
#include "w25qxx.h"
#include "w25qxx_spi.h"

/**/
void flashInit(void){
  W25qxx_SPI_Init();
	W25qxx_Init();
}
