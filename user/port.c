/*
Сюда вынесем настройки портов
*/
#include "port.h"
#include "allDefenition.h"

/*
Настраиваем вывод микроконтроллера.
Все включеия и переключения тактирования выполнять самостоятельно отдельными командами
*/
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
                PORT_MODE_TypeDef       PORT_MODE){
                    
    PORT_InitTypeDef port;                    
    PORT_StructInit(&port);
    port.PORT_Pin =         PORT_Pin;
    port.PORT_OE =          PORT_OE;       
    port.PORT_PULL_UP =     PORT_PULL_UP;      
    port.PORT_PULL_DOWN =   PORT_PULL_DOWN;    
    port.PORT_PD_SHM =      PORT_PD_SHM;       
    port.PORT_PD =          PORT_PD;           
    port.PORT_GFEN =        PORT_GFEN;         
    port.PORT_FUNC =        PORT_FUNC;        
    port.PORT_SPEED =       PORT_SPEED;        
    port.PORT_MODE =        PORT_MODE;


    PORT_Init(MDR_PORTx,&port);
                   
}
