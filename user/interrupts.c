/*
 *  Полный набор шаблонов обработчиков прерываний, основанный на предоставленном списке.
 *  ВНИМАНИЕ:  Замените "MDR_...", "USART_...", "EXTI_..." и другие имена на
 *              ФАКТИЧЕСКИЕ имена из документации к вашему микроконтроллеру!
 */
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

#include "interrupts.h"
#include "MDR32FxQI_timer.h"            // Milandr::Drivers:TIMER
#include "MDR32FxQI_bkp.h"              // Milandr::Drivers:BKP
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK

#include "allDefenition.h" 

extern uint32_t tickDelay ;
extern uint64_t sysTickCount;//?????????? ?????????? ???????
extern uint64_t sysTickGlobalTime;
extern uint8_t buzerEn ;
// Функция-заглушка для обработки необработанных прерываний (см. вектора прерываний)
void Default_Handler(void) {
    while (1) {
        // Зацикливание здесь указывает на непредвиденное прерывание.
        // Добавьте сюда отладочный код (например, запись об ошибке в UART).
    }
}

// -----------------------------------------------------------------------------
//  Основные обработчики
// -----------------------------------------------------------------------------


void NMI_Handler(void) {

}

void HardFault_Handler(void) {
//	    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF, ENABLE);

//    PORT_InitTypeDef PORT_InitStructure;
//    PORT_StructInit(&PORT_InitStructure);
//	PORT_InitStructure.PORT_Pin   = MCU_LCD_LED_PIN;
//    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
//	PORT_InitStructure.PORT_FUNC = PORT_FUNC_PORT;
//	PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
//    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
//    PORT_Init(MCU_LCD_LED_PORT, &PORT_InitStructure);
//		PORT_SetBits(MCU_LCD_LED_PORT,MCU_LCD_LED_PIN );	
//	
//PORT_SetBits(MCU_LCD_LED_PORT,MCU_LCD_LED_PIN);	
while(1){
}
}


void SVC_Handler(void) {

}


void PendSV_Handler(void) {

}


void SysTick_Handler(void) {
//	sysTickCount++;
//	if(tickDelay)
//		tickDelay--;
}


// -----------------------------------------------------------------------------
//  Обработчики внешних прерываний (замените имена и условия)
// -----------------------------------------------------------------------------

void MIL_STD_1553B2_IRQHandler(void) {
    // Обработчик прерывания MIL-STD-1553B2.
    // ... (Ваш код обработки MIL-STD-1553B2)
}

void MIL_STD_1553B1_IRQHandler(void) {
    // Обработчик прерывания MIL-STD-1553B1.
    // ... (Ваш код обработки MIL-STD-1553B1)
}

void USB_IRQHandler(void) {
    // Обработчик прерывания USB.
    // Проверка причины прерывания USB (например, завершение передачи, ошибка и т.д.)
    // if (USB_GetITStatus(USB, USB_IT_XXX) != RESET) {  // Замените USB_IT_XXX
    //     ... (Ваш код обработки прерывания USB)
    //     USB_ClearITPendingBit(USB, USB_IT_XXX);        // Очистите флаг прерывания!
    // }
}

void CAN1_IRQHandler(void) {
    // Обработчик прерывания CAN1.
    // ... (Ваш код обработки прерывания CAN1)
}

void CAN2_IRQHandler(void) {
    // Обработчик прерывания CAN2.
    // ... (Ваш код обработки прерывания CAN2)
}

void DMA_IRQHandler(void) {
    // Обработчик прерывания DMA.
    // Проверка причины прерывания DMA (например, завершение передачи, ошибка и т.д.)
    // ... (Ваш код обработки прерывания DMA)
}

void UART1_IRQHandler(void) {
//    if (USART_GetITStatus(MDR_UART1, USART_IT_RXNE) != RESET) { // Прерывание по приему данных
//        uint16_t data = USART_ReceiveData(MDR_UART1);  // Читаем данные

//        // Обработка полученных данных (пример)
//        // ...

//        USART_ClearITPendingBit(MDR_UART1, USART_IT_RXNE); // Очищаем флаг прерывания
//    }
    // Можно добавить обработку других типов прерываний UART (ошибка, передача завершена и т.д.)
}

void UART2_IRQHandler(void) {
//    if (USART_GetITStatus(MDR_UART2, USART_IT_RXNE) != RESET) { // Прерывание по приему данных
//        uint16_t data = USART_ReceiveData(MDR_UART2);  // Читаем данные

//        // Обработка полученных данных (пример)
//        // ...

//        USART_ClearITPendingBit(MDR_UART2, USART_IT_RXNE); // Очищаем флаг прерывания
//    }
    // Можно добавить обработку других типов прерываний UART (ошибка, передача завершена и т.д.)
}

void SSP1_IRQHandler(void) {
    // Обработчик прерывания SSP1 (SPI).
    // ... (Ваш код обработки прерывания SSP1)
}

void BUSY_IRQHandler(void) {
    // Обработчик прерывания BUSY.  (Что означает это прерывание, зависит от вашего оборудования.)
    // ... (Ваш код обработки прерывания BUSY)
}

void ARINC429R_IRQHandler(void) {
    // Обработчик прерывания ARINC429 Receiver.
    // ... (Ваш код обработки прерывания ARINC429 Receiver)
}

void POWER_IRQHandler(void) {
    // Обработчик прерывания POWER.  (Прерывание, связанное с управлением питанием.)
    // ... (Ваш код обработки прерывания POWER)
}

void WWDG_IRQHandler(void) {
    // Обработчик прерывания Window Watchdog Timer.
    // ... (Ваш код обработки прерывания WWDG)
}

void TIMER4_IRQHandler(void) {
    // Проверка статуса прерывания
//    if (TIMER_GetITStatus(MDR_TIMER4, TIMER_STATUS_CNT_ARR) != RESET) {
//        // Код обработки прерывания Timer 4
//        // ...

//        // Очистка флага прерывания
//        TIMER_ClearITPendingBit(MDR_TIMER4, TIMER_STATUS_CNT_ARR);
//    }
}

void TIMER1_IRQHandler(void) {
    // Проверка статуса прерывания
    if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ARR) != RESET) {
        // Код обработки прерывания Timer 1
	sysTickCount++;
	sysTickGlobalTime++;
	if(tickDelay)
		tickDelay--;
        // Очистка флага прерывания
        TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
    }
}

void TIMER2_IRQHandler(void) {
    static uint8_t beep = 0; // Пример статической переменной для состояния
    // Проверка статуса прерывания (замените условие).
    if (TIMER_GetITStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) != RESET) {
        // Код обработки прерывания Timer 2
		// Î÷èùàåì áèò ïðåðûâàíèÿ
		TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
	if(!!buzerEn){
        beep ^= 1;
        
    }
    else{
        beep = 0;
    
    }
	PORT_WriteBit(PORT_BUZZER, PIN_BUZZER,beep);
    }
}

void TIMER3_IRQHandler(void) {
//    // Проверка статуса прерывания
//    if (TIMER_GetITStatus(MDR_TIMER3, TIMER_STATUS_CNT_ARR) != RESET) {
//        // Код обработки прерывания Timer 3
//        // ...

//        // Очистка флага прерывания
//        TIMER_ClearITPendingBit(MDR_TIMER3, TIMER_STATUS_CNT_ARR);
//    }
}

void ADC_IRQHandler(void) {
//    if (ADC_GetITStatus(MDR_ADC, ADC_IT_EOC) != RESET) { // Прерывание по окончании преобразования
//        uint16_t data = ADC_GetConversionValue(MDR_ADC); // Читаем результат

//        // Обработка результата АЦП
//        // ...

//        ADC_ClearITPendingBit(MDR_ADC, ADC_IT_EOC); // Очищаем флаг прерывания
//    }
}

void ETHERNET_IRQHandler(void) {
    // Обработчик прерывания Ethernet.
    // ... (Ваш код обработки прерывания Ethernet)
}

void SSP3_IRQHandler(void) {
    // Обработчик прерывания SSP3 (SPI).
    // ... (Ваш код обработки прерывания SSP3)
}

void SSP2_IRQHandler(void) {
    // Обработчик прерывания SSP2 (SPI).
    // ... (Ваш код обработки прерывания SSP2)
}

void ARINC429T1_IRQHandler(void) {
    // Обработчик прерывания ARINC429 Transmitter 1.
    // ... (Ваш код обработки прерывания ARINC429 Transmitter 1)
}

void ARINC429T2_IRQHandler(void) {
    // Обработчик прерывания ARINC429 Transmitter 2.
    // ... (Ваш код обработки прерывания ARINC429 Transmitter 2)
}

void ARINC429T3_IRQHandler(void) {
    // Обработчик прерывания ARINC429 Transmitter 3.
    // ... (Ваш код обработки прерывания ARINC429 Transmitter 3)
}

void ARINC429T4_IRQHandler(void) {
    // Обработчик прерывания ARINC429 Transmitter 4.
    // ... (Ваш код обработки прерывания ARINC429 Transmitter 4)
}

void BKP_IRQHandler(void) {
//  if (BKP_RTC_GetFlagStatus(BKP_RTC_FLAG_SECF) == SET)
//    {
//    }
//    
//    
//    if (BKP_RTC_GetFlagStatus(BKP_RTC_FLAG_ALRF)==SET)
//    {
//       
//    }
//    BKP_RTC_ClearFlagStatus(BKP_RTC_FLAG_SECF | BKP_RTC_FLAG_ALRF);
}

void EXT_INT1_IRQHandler(void) {
//    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
//        // Код обработки внешнего прерывания по линии 1

//        // Очистка флага прерывания.  Это критично!
//        EXTI_ClearITPendingBit(EXTI_Line1);
//    }
}

void EXT_INT2_IRQHandler(void) {
//    if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
//        // Код обработки внешнего прерывания по линии 2

//        // Очистка флага прерывания.  Это критично!
//        EXTI_ClearITPendingBit(EXTI_Line2);
//    }
}

void EXT_INT3_IRQHandler(void) {
//    if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
//        // Код обработки внешнего прерывания по линии 3

//        // Очистка флага прерывания.  Это критично!
//        EXTI_ClearITPendingBit(EXTI_Line3);
//    }
}

void EXT_INT4_IRQHandler(void) {
//    if (EXTI_GetITStatus(EXTI_Line4) != RESET) {
//        // Код обработки внешнего прерывания по линии 4

//        // Очистка флага прерывания.  Это критично!
//        EXTI_ClearITPendingBit(EXTI_Line4);
//    }
}

