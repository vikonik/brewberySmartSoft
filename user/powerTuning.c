#include "powerTuning.h"
#include "mainPage.h"
#include "delay.h"
#include "allDefenition.h"
#include <stdbool.h>
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT

#include "ST7567_FB.h"
#include <stdio.h>
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"

extern void (*mainProcess)(void);
extern void functionNull(void);

char powerBuf[32];
static uint32_t powerTuningTimer = 0;
static uint8_t power = 0;
/**/
void powerTuning(uint8_t power){
	allMenuTextClear();
}


uint8_t currentPower = 0;
unsigned long previousMillis = 0;
bool relayState = false;
uint16_t relayOnTime = 0;
uint16_t relayOffTime = 0;

uint8_t setPower(uint8_t power) {
    // Проверка допустимости значения power
    if (power > 6) {
        power = 6;
    }
    
		if(currentPower == power)
			return power;
    
		currentPower = power;
    // Расчет времени работы реле (в миллисекундах)
    relayOnTime = power * 10 * 1000; // power * 10 секунд в миллисекундах
    relayOffTime = 60000 - relayOnTime; // Оставшееся время периода (60 секунд)
    
    // Сброс таймера при изменении мощности
    previousMillis = millis();
    relayState = true; // Начинаем с включенного состояния
     PORT_SetBits(RELAY_PORT, HEAT_1); // Включить реле
		
		if(currentPower != 0){
			mainProcess = updateRelay;
		allMenuTextClear();
		}
		else{
		mainProcess = functionNull;
				PORT_ResetBits(RELAY_PORT, HEAT_1);
				PORT_ResetBits(RELAY_PORT, COOL_1);
				PORT_ResetBits(RELAY_PORT, MIXER);
			allMenuTextClear();
			initMenuMainPage();
		}
		return power;
}

// Функция для обновления состояния реле, должна вызываться в loop()
void updateRelay(void) {
    unsigned long currentMillis = millis();
    unsigned long elapsedTime = currentMillis - previousMillis;
    
    if (relayState) {
        // Реле включено - проверяем, не истекло ли время работы
        if (elapsedTime >= relayOnTime && relayOnTime > 0) {
            relayState = false;
             PORT_ResetBits(RELAY_PORT, HEAT_1);// Выключить реле
            previousMillis = currentMillis;
        }
    } else {
        // Реле выключено - проверяем, не истекло ли время паузы
        if (elapsedTime >= relayOffTime && relayOffTime > 0) {
            relayState = true;
             PORT_SetBits(RELAY_PORT, HEAT_1); // Включить реле
            previousMillis = currentMillis;
        }
    }
		sprintf(powerBuf,"%s %d", stringSetPower, relayOnTime/10000 );
		ST7567_FB_fillRect(1, 32, 120, 13, 0);
		ST7567_FB_printText(1, 32, (char*)powerBuf, NORMAL);
		ST7567_FB_display();

}







