#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Milandr::Drivers:RST_CLK
#include "MDR32FxQI_bkp.h"              // Milandr::Drivers:BKP

#include "MDR32_rtc.h"


void RTCInit(void)
{
//	PORT_InitTypeDef PORT_InitStructure;
//	
//	//PE6 - OSC_IN32, PE7 - OSC_OUT32
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE,ENABLE);
//	
//	PORT_InitStructure.PORT_Pin = (PORT_Pin_6 | PORT_Pin_7);
//  PORT_InitStructure.PORT_MODE  = PORT_MODE_ANALOG;
//  PORT_Init(MDR_PORTE, &PORT_InitStructure);
	
	/* Enables the HSI clock for BKP control */
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP,ENABLE);
	
	/* Deinit BKP */
  BKP_DeInit();
	
	/* RTC reset */
  BKP_RTC_Reset(ENABLE);
  BKP_RTC_Reset(DISABLE);
	
	/* Configure LSE as RTC clock source */
	//LSE - внешний 32к√ц кварц
  RST_CLK_LSEconfig(RST_CLK_LSE_ON);				//TBD ѕочему-то не поднимаетс¤ флаг LSE_RDY
  while (RST_CLK_LSEstatus() != SUCCESS);
  BKP_RTCclkSource(BKP_RTC_LSEclk);	
	
	/* Enable all RTC interrupts */
  //BKP_RTC_ITConfig(BKP_RTC_IT_ALRF, ENABLE);		//‘лаг разрешени¤ прерывани¤ по совпадению основного счетчика и регистра RTC_ALRM
  //BKP_RTC_ITConfig(BKP_RTC_IT_SECF, ENABLE);		//‘лаг разрешени¤ прерывани¤ по разрешению счета основного счетчика от счетчика предварительного делени¤
  //BKP_RTC_ITConfig(BKP_RTC_IT_OWF, ENABLE);			//‘лаг разрешени¤ прерывани¤ по переполнени¤ основного счетчика RTC_CNT
  //NVIC_EnableIRQ(BKP_IRQn);
	
	/* Set the RTC counter value */
  BKP_RTC_WaitForUpdate();
  BKP_RTC_SetCounter(COUNT_VALUE);
	
	/* Set the RTC prescaler value */
  BKP_RTC_WaitForUpdate();
	BKP_RTC_SetPrescaler(PRESC_VALUE_LS);			//32768
	
	/* Set the RTC alarm value */
//    BKP_RTC_WaitForUpdate();
//    BKP_RTC_SetAlarm(ALARM_VALUE);

  /* RTC enable */
  BKP_RTC_WaitForUpdate();
  BKP_RTC_WorkPermit(ENABLE);
}

bool IsSecondFlagSet(void)
{	
	if (BKP_RTC_GetFlagStatus(BKP_RTC_FLAG_SECF)==SET)
	{
		MDR_BKP->RTC_CS |= BKP_RTC_CS_SECF;			//»з документации: "—брос флага осуществл¤етс¤ записью 1"
		return true;
	}
	return false;
}

bool IsAlarmFlagSet(void)
{	
	if (BKP_RTC_GetFlagStatus(BKP_RTC_FLAG_ALRF)==SET)
	{
		MDR_BKP->RTC_CS |= BKP_RTC_CS_ALRF;			//»з документации: "—брос флага осуществл¤етс¤ записью 1"
		return true;
	}
	return false;
}
