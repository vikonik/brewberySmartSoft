#include "mainPage.h"
#include "allDefenition.h"
#include "ST7567_FB.h"
#include "graph.h"
#include "delay.h"
#include <stdio.h>
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"
#include "menu.h"
#include "menuClearing.h"
#include "menuRecept.h"
#include "menuManualControl.h"
#include "menuSetting.h"


extern menu_t menu_SetTime;
static const uint8_t firstStringH = 15;

MenuText_t label_Recept = {stringRecept, 1, firstStringH};
MenuText_t label_Clearing = {stringClearing, 1, firstStringH + charHeight8pt};
MenuText_t label_ManualControl = {stringManualControl, 1, firstStringH + charHeight8pt*2};
MenuText_t label_Setting = {stringSetting, 1, firstStringH + charHeight8pt*3};




MENU_ADD(menu_Recept,             menu_Clearing,  				m_null,       			m_null,  printMenuReceptMain,   		NULL,      &label_Recept);
MENU_ADD(menu_Clearing ,          menu_ManualControl,   	menu_Recept,        m_null,  printMenuClearingState_1,  NULL,      &label_Clearing);
MENU_ADD(menu_ManualControl ,     menu_Setting,   				menu_Clearing,      m_null,  printMenuManualControl,   	NULL,      &label_ManualControl);
MENU_ADD(menu_Setting,            m_null,         				menu_ManualControl, m_null,  printMenuSetting,   				NULL,      &label_Setting);

/**/
void printMainPage(DeviceStatus_t *status){
//	  static uint64_t pictoBlink = 0;
	static uint8_t pictoTrig = 0;
	static uint64_t timeToShowPage = 0;
	const uint8_t wifi_x = 127 - image_data_WiFi[0];
	const uint8_t wifi_y = 1;
	const uint8_t bt_x = wifi_x;
	const uint8_t bt_y = 1;
	const uint8_t lock_x = bt_x - image_data_Lock[0]-2;
	const uint8_t lock_y = 1;	
	uint64_t timeRaw = getGlobalTime();
	timeRaw /= 60000; //это столько натикало минут
	uint8_t hh = timeRaw / 60;
	hh  %= 24;
	uint8_t mm = timeRaw % 60;
	char _buf[7];
	static uint8_t secTrigger = 0;
	if(millis() - timeToShowPage < 1000 )
		return;
	
	timeToShowPage = millis();
	sprintf(_buf, "%02d%s%02d", hh,  (secTrigger ^= 1) ? ":" : " " ,mm);//Секундной точкой управляем здесь
	
	
	//Символ блокировки
	if(!!status->isLocked){
		ST7567_FB_drawBitmap(image_data_Lock,lock_x,lock_y, NORMAL);
	}
	else{
		ST7567_FB_clerBitmap(image_data_Lock,lock_x,lock_y);
	}

	//Символ подключения к сети
	if(!!status->wifiStatus & !!status->isConnected){
		ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
		ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
		ST7567_FB_drawBitmap(image_data_WiFi,wifi_x,wifi_y, NORMAL);
	}
	else if(!!status->btStatus & !!status->isConnected){
		ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
		ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
		ST7567_FB_drawBitmap(image_data_BT,bt_x,bt_y, NORMAL);
	}
	else if(!!status->wifiStatus & !status->isConnected){
		if(pictoTrig){
			pictoTrig ^= 1;
			ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
			ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
			ST7567_FB_drawBitmap(image_data_WiFi,wifi_x,wifi_y, NORMAL);
		}
		else{
			pictoTrig ^= 1;
			ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
			ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);			
		}
	}
	else if(!!status->btStatus & !status->isConnected){
		ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
		ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
		ST7567_FB_drawBitmap(image_data_BT,bt_x,bt_y, NORMAL);
		if(pictoTrig){
			pictoTrig ^= 1;
			ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
			ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
			ST7567_FB_drawBitmap(image_data_BT,bt_x,bt_y, NORMAL);
		}
		else{
			pictoTrig ^= 1;
			ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
			ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
			}
	}
	else{
		ST7567_FB_clerBitmap(image_data_WiFi,wifi_x,wifi_y);
		ST7567_FB_clerBitmap(image_data_BT,bt_x,bt_y);
	}
	
	//Часы
	ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
	ST7567_FB_fillRect(48, 1,  32, 13, 0);//Сотрем старое значение
  ST7567_FB_printText(ST7567_FB_align_text(_buf, ALIGN_CENTR), 1, _buf, NORMAL);
	//линия под заголовком
	ST7567_FB_drawLineH(0, 127, 14, 1);
	
	 
	
	//Пункты меню 
//	ST7567_FB_printText(1, 15, "Рецепт", NORMAL);
//	ST7567_FB_printText(1, 15+13, "Очистка", NORMAL);
//	ST7567_FB_printText(1, 15+13*2, "Настройка", NORMAL);
	ST7567_FB_display();
}

/**/
void initMenuMainPage(void){
	deviceStatus.isPowerSetting = 1;
	MENU_Init(&menu_Recept);
}

/**/
void allMenuTextClear(void){
	ST7567_FB_fillRect(1, 1, 48, 13, 0);//Стираем верхний левый угол
	ST7567_FB_fillRect(0, firstStringH,  128, 64-firstStringH, 0);//Сотрем старое значение
	ST7567_FB_display();
}
