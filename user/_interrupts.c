/* Includes ------------------------------------------------------------------*/
#include "interrupts.h"
#include "K1986VE9xI.h"                 // Device header


//#include "common.h"
//#include "timers.h"
//#include "esp.h"
//#include "MPR121.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//extern volatile u8 refreshDisplayFlag;
//extern volatile u64 tick;

//extern s_hotpanel hotpanel;

//extern s_uart uart_esp;
//extern mk_data_s mk_data_out;

////�������
//extern s_TimerSoft timerPowerOnIdle;
//extern s_TimerSoft timerActivity;
//extern s_TimerSoft timerPlusMinusHold;
//extern s_TimerSoft timerPlusMinusPeriod;
//extern s_TimerSoft timerOneSecond;
//extern s_TimerSoft timerBurnerTimerStartDelay;
//extern s_TimerSoft timerBeepOn;
//extern s_TimerSoft timerBeepOff;
//extern s_TimerSoft timerBurnerTimerFinishedAlarm;
//extern s_TimerSoft timerUnlockMode;

//extern void InputRelaySquareWaveGenerator(void);
//extern u8 GetTemperature(float);

extern uint32_t tickDelay ;
extern uint64_t sysTickCount;//?????????? ?????????? ???????

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
//	volatile static bool _continue = true;
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	sysTickCount++;
	if(tickDelay)
		tickDelay--;
		
//	tick++;
//	
//	// �������������� �������� ���� ���������� �������� ������ 1 ����
//	if(timerPowerOnIdle.enabled)
//		timerPowerOnIdle.value++;
//	if(timerActivity.enabled)
//		timerActivity.value++;
//	if(timerPlusMinusHold.enabled)
//		timerPlusMinusHold.value++;
//	if(timerPlusMinusPeriod.enabled)
//		timerPlusMinusPeriod.value++;
//	
//	if(timerOneSecond.enabled)
//		timerOneSecond.value++;
//	if(timerBurnerTimerStartDelay.enabled)
//		timerBurnerTimerStartDelay.value++;
//	if(timerBeepOn.enabled)
//		timerBeepOn.value++;
//	if(timerBeepOff.enabled)
//		timerBeepOff.value++;
//	if(timerBurnerTimerFinishedAlarm.enabled)
//		timerBurnerTimerFinishedAlarm.value++;
//	if(timerUnlockMode.enabled)
//		timerUnlockMode.value++;
}


//UART
void Timer2_IRQHandler(void) 
{
//	if (TIMER_GetITStatus(MDR_TIMER2, TIMER_STATUS_CNT_ARR) != RESET)
//  {
//		// ������� ��� ����������
//		TIMER_ClearFlag(MDR_TIMER2, TIMER_STATUS_CNT_ARR);
//	
//		uart_esp.rx_counter = 0;
//		uart_esp.receiveFinished = true;	
//	}
}

//LED
void Timer3_IRQHandler(void) 
{
//	if (TIMER_GetITStatus(MDR_TIMER3, TIMER_STATUS_CNT_ARR) != RESET)
//  {	
//		// ������� ��� ����������
//		TIMER_ClearFlag(MDR_TIMER3, TIMER_STATUS_CNT_ARR);	
//		refreshDisplayFlag = true; 									//���� ���������, ������ ���� ������ ���������
//	}
//	//InputRelaySquareWaveGenerator();							//�������� ����. TBD, ��������, �������� ��������� ��� ������� � ������ ������ (������� ����� ����������� ����)
} 

//�������
void Timer1_IRQHandler(void) 
{

//	if (TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ARR) != RESET)
//  {
//		// ������� ��� ����������
//		TIMER_ClearFlag(MDR_TIMER1, TIMER_STATUS_CNT_ARR);
//		
//		if(hotpanel.buzzer.Mute)
//			PIO_ResetBits(PORT_BUZ, PIN_BUZ);																		//� ���������� ������ �� ������� ����� ������ 0
//		else
//			PIO_ToogleBit(PORT_BUZ, PIN_BUZ);                             			//������ ������ �� �������
//		
//		//InputRelaySquareWaveGenerator();
//	}
}

void UART1_IRQHandler(void)
{
	//�������� ������	
//	if (UART_GetITStatus(MDR_UART1, UART_IT_TX)) 
//	{
//		UART_ClearITPendingBit(MDR_UART1, UART_IT_TX);		
//		
//		uart_esp.tx_counter++;																							//����������� ������� ���������� ������������ ����
//		if(uart_esp.tx_counter == uart_esp.bytesToSend)											//���� ��� ������ ����������,
//		{
//			uart_esp.sendFinished = true;																			//�� ������������� ������� ��������
//			uart_esp.tx_counter = 0;																					//� ���������� �������
//			uart_esp.bytesToSend = 0;
//		}

//		if(uart_esp.sendFinished == false)																	//���� �������� �� �����������, �������� ��������� ����
//			UART_SendData(MDR_UART1, uart_esp.buf_tx[uart_esp.tx_counter]);		
//	}
//	
//	//���� ������
//	if (UART_GetITStatus(MDR_UART1, UART_IT_RX)) 
//	{
//		UART_ClearITPendingBit(MDR_UART1, UART_IT_RX);	 										//������� ���� ����������
//		UART_PauseNill(ESP_UART_TIMER);																			//��������� ������ (����� ��� ����������� ����� ������) 
//		
//		uart_esp.buf_rx[uart_esp.rx_counter] = (uint8_t) MDR_UART1->DR;
//		uart_esp.rx_counter++;
//		
//#ifdef JSON_PACKET
//		if(uart_esp.buf_rx[uart_esp.rx_counter-1] == '}')										//���� ������� �������� � ���� JSON ������, �� '}' - ��������� ����� ������
//#else
//		if(uart_esp.rx_counter == sizeof(mk_data_s))												//���� ������� �������� � ������ �������, �� ��� ���� �� ������ ������ �������� �� ��������� ������
//#endif
//		{
//			uart_esp.receivedDataOfValidLength = true;
//		}		

//		if (uart_esp.rx_counter >= UART_BUFF_SIZE)
//		{  
//			uart_esp.rx_counter = 0; 
//		}
//	}
} 

void ADC_IRQHandler(void)
{
//	float voltage;
//	float thermistor_resistance;
//	
//	if(MDR_ADC->ADC1_STATUS & ADC1_FLAG_END_OF_CONVERSION)
//	{		
//		voltage = (float)( ADC1_GetResult() & 0x0FFF)/4095*3.3;					//������ 0x0FFF ����� �����������, �.�. ADC1_RESULT �������� � ������� ������ ����� ������, ������� ����� �������� 
//																																		//��. ����� https://support.milandr.ru/base/primenenie/programmirovanie-32-razryadnykh-mk/rabota_s_atsp/48510/
//		//��������� �������� ������������� ��������������
//		if(voltage > 0 && voltage < 3.3)
//		{
//			thermistor_resistance = 3.3*4700/voltage - 4700;
//		}
//		
//		//��������� �������� ����������� � �������� �������
//		hotpanel.temperature = GetTemperature(thermistor_resistance);
//	}	
}

void MPR121_IRQHandler(void)
{
}

