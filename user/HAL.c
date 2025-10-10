#include "HAL.h"
#include "MDR32FxQI_timer.h"            // Milandr::Drivers:TIMER
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_ssp.h"              // Milandr::Drivers:SSP

#include "port.h"
#include "delay.h"
#include "allDefenition.h"
#include "I2C_soft.h"
#include "MPR121.h" 
#include "MDR32_inout.h"
#include "delay.h"
#include "ds18b20.h"
#include "tempSensor.h"
#include "ST7567_FB.h"
#include "graph.h"
#include "mainPage.h"
#include "manifest.h"
#include "hc595.h"
#include "json.h"


xI2C I2C_b;										// Структура софтварного I2C (нужен для опроса кнопок с микросхемы MPR121)
bool MPR121initRresult = false;

DeviceStatus_t deviceStatus;
extern uint8_t buzerEn;
extern uint8_t displayArray[];
extern uint8_t digit[];
extern uint16_t allButtonsRAW;

//Приватные функуии
void initCLK(void);
void initMainTimer(void);
void initBuzerTimer(void);
void initRelay(void);
void initBuzzer(void);
void initSPI(void);
void initResetPin(void);
void initLCDLed(void);

void (*mainProcess)(void);
void functionNull(void){}

//Глобальные функции
/**/
void initDevice(void){
	deviceStatus.temperatureCurrent = 25;
	deviceStatus.manualControlCurrentData.targetTimer_h = 22;
	deviceStatus.manualControlCurrentData.targetTimer_m = 15;
	
	
	initCLK();
	SystemCoreClockUpdate();
	initMainTimer();//Вместо SysTick
	/* Все порты тактируются здесь, если дужно то выдернуть отсюда */
    RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_SSP1 |RST_CLK_PCLK_SSP2 |
                    RST_CLK_PCLK_PORTB |RST_CLK_PCLK_PORTC | RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTE | RST_CLK_PCLK_PORTF | RST_CLK_PCLK_PORTA), ENABLE);
//SysTick_Config(SystemCoreClock/1000);//Системный таймен настроим на 1 милисекунду

	//displayPinTest();
	
	initRelay();
	initBuzerTimer();	
	initBuzzer();	
	
	initLCDLed();
	initResetPin();	
	initSPI();
	
//	ST7567_FB_init(5);
	PORT_WriteBit(LCD_RST_PORT, LCD_RST_PIN, (FlagStatus)SET);
	delay_ms(50);
	PORT_WriteBit(LCD_RST_PORT, LCD_RST_PIN, (FlagStatus)RESET);
	delay_ms(500);
	PORT_WriteBit(LCD_RST_PORT, LCD_RST_PIN, (FlagStatus)SET);
	delay_ms(50);
	
	ST7567_FB_initCmds();
  ST7567_FB_setContrast(10);
  ST7567_FB_setRotation(0);
	ST7567_FB_cls();
  ST7567_FB_display();
	ST7567_FB_displayOn(true);
	ST7567_FB_cls();
//  ST7567_FB_drawBitmap(image_data_Lock,2,1, NORMAL);//image_data_temperatureControl
//	ST7567_FB_display();
	printMainPage(&deviceStatus);
	
	HC595_Init(1);
	
	tempSensorInit();
  
	WiFi_uartInit();//Настраиваем UART для WiFi
	uart_parser_init(&uart_parser);
	
	I2C_b = I2Csft_Settings();								//Заполнение структуры I2C данными
	I2Csft_Init(&I2C_b);											//Инициализация портов ввода-вывода
	MPR121initRresult = mpr121_init(0x5A);
	beep(1);//Сигнал о готовности
}

/**/
void initCLK(void){
    RST_CLK_DeInit();
    SystemCoreClockUpdate();
	    RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTC | RST_CLK_PCLK_EEPROM), ENABLE);

//    /* Setup internal DUcc voltage regulator work mode based on clock frequency. Added for demonstration, since the same is already done in RST_CLK_DeInit(). */
//    BKP_DUccMode(BKP_DUcc_upto_10MHz);

//    /* Setup EEPROM access delay to 0 because HSI = 8MHz < 25MHz.
//       EEPROM access delay sould be set before the clock frequency increases or
//       after the clock frequency decreases. */
//    EEPROM_SetLatency(EEPROM_Latency_0);
	
	    /* Set RST_CLK to default */
    RST_CLK_DeInit(); // Also resets DUcc voltage regulator work mode
    SystemCoreClockUpdate();
	RST_CLK_PCLKcmd((RST_CLK_PCLK_PORTC | RST_CLK_PCLK_EEPROM), ENABLE);
	

	//Запускаемся от внешнего кварца
	/* Enable HSE clock oscillator */
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
    if (RST_CLK_HSEstatus() == SUCCESS)                     /* Good HSE clock */
    {
        /* Select HSE clock as CPU_PLL input clock source */
        /* Set PLL multiplier to 7                        */
        RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul8 );//RST_CLK_CPU_PLLmul7
        /* Enable CPU_PLL */
        RST_CLK_CPU_PLLcmd(ENABLE);
        if (RST_CLK_CPU_PLLstatus() == SUCCESS)             /* Good CPU PLL */
        {
            /* Set CPU_C3_prescaler to 2 */
            RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
            /* Set CPU_C2_SEL to CPU_PLL output instead of CPU_C1 clock */
            RST_CLK_CPU_PLLuse(ENABLE);

//            /* Setup internal DUcc voltage regulator work mode based on clock frequency */
//            BKP_DUccMode(BKP_DUcc_upto_40MHz);
//            /* Setup EEPROM access delay to 1: 7*HSE/2 = 28MHz > 25MHz */
//            EEPROM_SetLatency(EEPROM_Latency_1);

            /* Select CPU_C3 clock on the CPU clock MUX */
            RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);

            SystemCoreClockUpdate();
//SystemCoreClock
            /* LED1 blinking with 7*HSE/2 clock as input clock source */
            //BlinkLED1(BLINK_NUM, BLINK_DELAY);
        }
        else                                                /* CPU_PLL timeout */
        {
            //IndicateError();
        }
    }
    else                                                    /* HSE timeout */
    {
        //IndicateError();
    }
}

/*
Системный таймер считает не правильно. Настроим основной
*/
void initMainTimer(void){
	TIMER_CntInitTypeDef sTIM_CntInit;
	SystemCoreClockUpdate();
	/* Enable peripheral clocks */
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_TIMER1 ), ENABLE);   
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER1);

	/* Initializes the TIMERx Counter */
	sTIM_CntInit.TIMER_Prescaler        = 0;
	sTIM_CntInit.TIMER_Period           = 1000-1;
	sTIM_CntInit.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource      = TIMER_EvSrc_TIM_CLK;
	sTIM_CntInit.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf   = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit(MDR_TIMER1,&sTIM_CntInit);

	/* Enable TIMER1 clock */
	TIMER_BRGInit(MDR_TIMER1,TIMER_HCLKdiv64);
	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ARR, ENABLE);

	NVIC_EnableIRQ(TIMER1_IRQn);

	/* Enable TIMER1 */
	TIMER_Cmd(MDR_TIMER1,ENABLE);  
}

/**/
void initBuzerTimer(void){
	TIMER_CntInitTypeDef sTIM_CntInit;
	SystemCoreClockUpdate();
	/* Enable peripheral clocks */
	RST_CLK_PCLKcmd((RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_TIMER2 ), ENABLE);   
	/* Reset all TIMER1 settings */
	TIMER_DeInit(MDR_TIMER2);

	/* Initializes the TIMERx Counter */
	sTIM_CntInit.TIMER_Prescaler        = 10;
	sTIM_CntInit.TIMER_Period           = 1000-1;
	sTIM_CntInit.TIMER_CounterMode      = TIMER_CntMode_ClkFixedDir;
	sTIM_CntInit.TIMER_CounterDirection = TIMER_CntDir_Up;
	sTIM_CntInit.TIMER_EventSource      = TIMER_EvSrc_TIM_CLK;
	sTIM_CntInit.TIMER_FilterSampling   = TIMER_FDTS_TIMER_CLK_div_1;
	sTIM_CntInit.TIMER_ARR_UpdateMode   = TIMER_ARR_Update_Immediately;
	sTIM_CntInit.TIMER_ETR_FilterConf   = TIMER_Filter_1FF_at_TIMER_CLK;
	sTIM_CntInit.TIMER_ETR_Prescaler    = TIMER_ETR_Prescaler_None;
	sTIM_CntInit.TIMER_ETR_Polarity     = TIMER_ETRPolarity_NonInverted;
	sTIM_CntInit.TIMER_BRK_Polarity     = TIMER_BRKPolarity_NonInverted;
	TIMER_CntInit(MDR_TIMER2,&sTIM_CntInit);

	/* Enable TIMER1 clock */
	TIMER_BRGInit(MDR_TIMER2,TIMER_HCLKdiv1);

	TIMER_ITConfig(MDR_TIMER2, TIMER_STATUS_CNT_ARR, ENABLE);

	NVIC_EnableIRQ(TIMER2_IRQn);

	/* Enable TIMER1 */
	TIMER_Cmd(MDR_TIMER2,ENABLE);  
}

/**/
void initRelay(void){
	PORT_InitTypeDef PORT_InitStructure;
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = MIXER | HEAT_1 |COOL_1;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(RELAY_PORT, &PORT_InitStructure);
}

/**/
void initBuzzer(void){
	PORT_InitTypeDef PORT_InitStructure;
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = PIN_BUZZER;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
	PORT_Init(PORT_BUZZER, &PORT_InitStructure);
}
		


/**/
SSP_InitTypeDef sSSP;
PORT_InitTypeDef PORT_InitStructure;
void initSPI(void){
		/* Configure SSP1 pins: MOSI,	SCK, 	nCS,	AO */
    /* Configure PORTF pins 	0, 		1, 		2, 	3 */
PORT_StructInit(&PORT_InitStructure);
	    PORT_InitStructure.PORT_Pin   = (LCD_CS_PIN| LCD_D_C_PIN);
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(LCD_CSK_PORT, &PORT_InitStructure);
	

    PORT_InitStructure.PORT_Pin   = (LCD_MOSI_PIN | LCD_CSK_PIN );
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_ALTER;
	    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(LCD_CSK_PORT, &PORT_InitStructure);

	
	/* Reset all SSP settings */
    SSP_DeInit(MDR_SSP1);


    SSP_BRGInit(MDR_SSP1, SSP_HCLKdiv2);


    /* SSP1 MASTER configuration */
    SSP_StructInit (&sSSP);

    sSSP.SSP_SCR        = 0x10;
    sSSP.SSP_CPSDVSR    = 2;
    sSSP.SSP_Mode       = SSP_ModeMaster;
    sSSP.SSP_WordLength = SSP_WordLength8b;
    sSSP.SSP_SPH        = SSP_SPH_1Edge;
    sSSP.SSP_SPO        = SSP_SPO_Low;
    sSSP.SSP_FRF        = SSP_FRF_SPI_Motorola;
    sSSP.SSP_HardwareFlowControl = SSP_HardwareFlowControl_None;
    SSP_Init (MDR_SSP1,&sSSP);
		    /* Enable SSP1 */
    SSP_Cmd(MDR_SSP1, ENABLE);


	}

/**/
void initResetPin(void){
	PORT_InitTypeDef PORT_InitStructure;
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = LCD_RST_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_RST_PORT, &PORT_InitStructure);
}

/**/
void initLCDLed(void){
	PORT_InitTypeDef PORT_InitStructure;
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = MCU_LCD_LED_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MCU_LCD_LED_PORT, &PORT_InitStructure);
	PORT_SetBits(MCU_LCD_LED_PORT,MCU_LCD_LED_PIN );	
}

/***************************************************/

/**/
void heatOn(void){
	PORT_SetBits(RELAY_PORT, HEAT_1);
}

/**/
void heatOff(void){
	PORT_ResetBits(RELAY_PORT, HEAT_1);
}
/**/
void nasosOn(void){
	PORT_SetBits(RELAY_PORT, MIXER);
}
/**/
void nasosOff(void){
	PORT_ResetBits(RELAY_PORT, MIXER);
}

/*
Функция ферментации
греем одну минуту, потом пауза 2
при нагреве насос работает 90 секунд
*/

/**/
// Глобальные переменные для управления временем
static uint64_t previousMillis = 0;
static bool heatEnabled = false;
static bool pumpEnabled = false;

void fermentationFunction(void)
{
    uint64_t currentMillis = millis(); // Функция получения времени
    uint64_t elapsedTime = currentMillis - previousMillis;
    
    // Запуск нового цикла
    if (elapsedTime >= 180000) // 180 секунд
    {
        heatOn();
        nasosOn();
        heatEnabled = true;
        pumpEnabled = true;
        previousMillis = currentMillis;
        return;
    }
    
    // Выключение нагрева через 60 секунд
    if (heatEnabled && elapsedTime >= 60000) // 60 секунд
    {
        heatOff();
        heatEnabled = false;
    }
    
    // Выключение насоса через 90 секунд
    if (pumpEnabled && elapsedTime >= 90000) // 90 секунд
    {
        nasosOff();
        pumpEnabled = false;
    }
}

void filtrationFunction(void)
{
    uint64_t currentMillis = millis(); // Функция получения времени
    uint64_t elapsedTime = currentMillis - previousMillis;
    
    // Запуск нового цикла
    if (elapsedTime >= 180000) // 180 секунд
    {
        heatOn();
        nasosOn();
        heatEnabled = true;
        pumpEnabled = true;
        previousMillis = currentMillis;
        return;
    }
    
    // Выключение нагрева через 120 секунд
    if (heatEnabled && elapsedTime >= 120000) // 120 секунд
    {
        heatOff();
        heatEnabled = false;
    }
    
    // Выключение насоса через 120 секунд
    if (pumpEnabled && elapsedTime >= 120000) // 120 секунд
    {
        nasosOff();
        pumpEnabled = false;
    }
}
void allFunctionStop(void){
	previousMillis = 0;
		heatOff();
		nasosOff();
}



/**/
// Глобальные переменные для управления
static uint32_t previousTempMillis = 0;
static uint32_t previousPumpMillis = 0;
//static bool heatEnabled = false;//Определены ранее
//static bool pumpEnabled = false;//Определены ранее
static bool pumpRunning = false;
static bool pumpForced = false;

void processTemperatureControl(float targetTemp, 
                              uint32_t pumpOnTime, 
                              uint32_t pumpOffTime)
{
    uint32_t currentMillis = millis();
    
    // Управление температурой
    if (currentMillis - previousTempMillis >= 1000) // Проверка каждую секунду
    {
        previousTempMillis = currentMillis;
        
        if (deviceStatus.temperatureCurrent < targetTemp)
        {
            if (!heatEnabled)
            {
                heatOn();
                heatEnabled = true;
            }
            pumpForced = false; // Сбрасываем принудительный режим
        }
        else
        {
            if (heatEnabled)
            {
                heatOff();
                heatEnabled = false;
            }
            
            // Если температура выше целевой - включаем насос постоянно
            if (!pumpForced)
            {
                nasosOn();
                pumpRunning = true;
                pumpForced = true;
            }
            return; // Прерываем дальнейшую обработку для циклического режима
        }
    }
    
    // Циклическое управление насосом (только когда температура ниже целевой)
    if (!pumpForced)
    {
        if (pumpRunning)
        {
            if (currentMillis - previousPumpMillis >= pumpOnTime)
            {
                nasosOff();
                pumpRunning = false;
                previousPumpMillis = currentMillis;
            }
        }
        else
        {
            if (currentMillis - previousPumpMillis >= pumpOffTime)
            {
                nasosOn();
                pumpRunning = true;
                previousPumpMillis = currentMillis;
            }
        }
    }
}

/*
Подача короткого звукового сигнала
num - количество
*/
void beep(uint8_t num){

	for(uint8_t i = 0; i < num; i++){
		buzerEn = 1;
		delay_ms(200);
		buzerEn = 0;	
	}
}

/************ Тест выводов дисплея ****************/
void displayPinTest(void){
	PORT_InitTypeDef PORT_InitStructure;
	
	PORT_StructInit(&PORT_InitStructure);
	PORT_InitStructure.PORT_Pin   = LCD_RST_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_RST_PORT, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = LCD_D_C_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_D_C_PORT, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = LCD_MOSI_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_MOSI_PORT, &PORT_InitStructure);
	
	PORT_InitStructure.PORT_Pin   = LCD_CSK_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_CSK_PORT, &PORT_InitStructure);
	
		PORT_InitStructure.PORT_Pin   = LCD_CS_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(LCD_CS_PORT, &PORT_InitStructure);
	
			PORT_InitStructure.PORT_Pin   = MCU_LCD_LED_PIN;
	PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
	PORT_InitStructure.PORT_MODE = PORT_MODE_DIGITAL;
	PORT_InitStructure.PORT_SPEED = PORT_SPEED_SLOW;
	PORT_Init(MCU_LCD_LED_PORT, &PORT_InitStructure);
	
	while(1){
	PORT_SetBits(LCD_RST_PORT, LCD_RST_PIN);
	PORT_SetBits(LCD_D_C_PORT, LCD_D_C_PIN);
	PORT_SetBits(LCD_MOSI_PORT, LCD_MOSI_PIN);
	PORT_SetBits(LCD_CSK_PORT, LCD_CSK_PIN);
	PORT_SetBits(LCD_CS_PORT, LCD_CS_PIN);
	PORT_SetBits(MCU_LCD_LED_PORT, MCU_LCD_LED_PIN);
	delay_ms(3000);
	
//	PORT_ResetBits(LCD_RST_PORT, LCD_RST_PIN);
//	PORT_ResetBits(LCD_D_C_PORT, LCD_D_C_PIN);
//	PORT_ResetBits(LCD_MOSI_PORT, LCD_MOSI_PIN);
//	PORT_ResetBits(LCD_CSK_PORT, LCD_CSK_PIN);
//	PORT_ResetBits(LCD_CS_PORT, LCD_CS_PIN);
//	PORT_ResetBits(MCU_LCD_LED_PORT, MCU_LCD_LED_PIN);
//		delay_ms(3000);
	}
}


/*
Вывод сообщения об ошибке
*/
void printTempSensorError(void){
	
	ST7567_FB_cls();
	ST7567_FB_printText(1, 1, (char*)tempSensorError, NORMAL);//	SWITCH
	ST7567_FB_display();
}

/*
Настройка UART для WiFi
*/
void WiFi_uartInit(void){
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
	//настроить порты, UART_TXD1 - PC3, UART_RXD1 - PC4; UART_TXD2 - PD13, UART_RXD2 - PD14
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	PORT_InitTypeDef PortInit;
	
	//RX
	PortInit.PORT_OE = PORT_OE_IN;										//вход	
	PortInit.PORT_Pin = PIN_WiFI_RX;									//номер пина
	PortInit.PORT_PULL_UP = PORT_PULL_UP_ON;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_OPEN;									//вроде как "open drain" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;						//как переопределенная функция
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	PORT_Init(PORT_WiFI, &PortInit);
	
	//TX
	PortInit.PORT_OE = PORT_OE_OUT;										//выход	
	PortInit.PORT_Pin = PIN_WiFI_TX;									//номер пина
	PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;					//pull up резистор
	PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;			//pull down резистор
	PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;						//триггер Шмитта
	PortInit.PORT_PD = PORT_PD_DRIVER;								//вроде как "push pull" режим
	PortInit.PORT_GFEN = PORT_GFEN_OFF;								//фильтр на короткие импульсы
	PortInit.PORT_FUNC = PORT_FUNC_OVERRID;							//как переопределенная функция
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;					//максимальная скорость
	PortInit.PORT_MODE = PORT_MODE_DIGITAL;						//цифровой
	PORT_Init(PORT_WiFI, &PortInit);
	
	UART_DeInit(WiFI_UART);
  UART_BRGInit(WiFI_UART,UART_HCLKdiv1);
	UART_InitTypeDef WiFi_uart;
  WiFi_uart.UART_BaudRate = 115200;
  WiFi_uart.UART_WordLength = UART_WordLength8b;
  WiFi_uart.UART_StopBits = UART_StopBits1;
  WiFi_uart.UART_Parity = UART_Parity_No;
  WiFi_uart.UART_FIFOMode = UART_FIFO_OFF;
	//WiFi_uart.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
  WiFi_uart.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE|UART_HardwareFlowControl_TXE;

  UART_Init(WiFI_UART, &WiFi_uart);
  
  UART_Cmd(WiFI_UART, ENABLE);
	UART_ITConfig(MDR_UART2, UART_IT_RX, ENABLE);	
	NVIC_EnableIRQ(UART2_IRQn);

}
