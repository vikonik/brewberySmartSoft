#ifndef _MDR32_RTC_H
#define _MDR32_RTC_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup

#include "stdbool.h"

#define COUNT_VALUE     0
//#define ALARM_VALUE     60
//#define PRESC_VALUE_HS  1000000
#define PRESC_VALUE_LS  32768
//#define RTCHSI_PRESC    RST_CLK_HSIclkDIV8
//#define RTCHSE_PRESC    RST_CLK_HSEclkDIV1

#ifdef __cplusplus
extern "C" {
#endif

void RTCInit(void);
bool IsSecondFlagSet(void);
bool IsAlarmFlagSet(void);

#ifdef __cplusplus
}
#endif	

#endif		//_MDR32_RTC_H
