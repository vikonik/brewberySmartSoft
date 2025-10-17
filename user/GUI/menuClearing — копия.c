#include "menuClearing.h"
#include "HAL.h"
#include "allDefenition.h"
#include "ST7567_FB.h"
#include "graph.h"
#include "delay.h"
#include <stdio.h>
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"
#include "menu.h"
#include "mainPage.h"

uint16_t woshingTime = 10;//20*60;
extern void (*mainProcess)(void);

static const uint8_t ferstStringH = 15;
void returnMenuClearingState_1(void);
void woshingProcessStart(void);
void woshingProcess(void);
void woshingSkip(void);
void rinsingProcess(void);
void rinsingProcessStart(void);
void disinfectionProcessStart(void);
void woshingComplite(void);
void disinfection(void);
void disinfectionProcessSkip(void);
void rinsingProcessSkip(void);
	
MenuText_t label_WoshingState_1 = {stringWoshingState_1, 1, ferstStringH*1};
MenuText_t label_Woshing = {stringWoshing, 1, ferstStringH*1};

void printMenuClearingState_1(void){
	
	menu_Nazad.next = &m_null;
	menu_Nazad.previous = &menu_Dalshe;
  //menu_Nazad.select = &menu_Recept;
	menu_Nazad.func = returnMenuClearingState_1;
	label_Nazad.posX = 16; label_Nazad.posY = 50;label_Nazad.text = stringOtmena;
	
	menu_Dalshe.next =  &menu_Nazad;
	menu_Dalshe.previous = &m_null;
	menu_Dalshe.func = woshingProcessStart;
	label_Dalshe.posX = 80; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringBegin;
	
	ST7567_FB_cls();
        ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
 //       ST7567_FB_printText(1, (cfont.CharacterHeight-1) * item_pos, (char*)item->text, INVERSE); //(i ==0) ? INVERSE :NORMAL 
        ST7567_FB_printText(label_WoshingState_1.posX, label_WoshingState_1.posY, (char*)label_WoshingState_1.text, NORMAL);

	
	ST7567_FB_display();
	MENU_Init(&menu_Nazad);

}

/*
Возвращаемся на главный экран
*/
void returnMenuClearingState_1(void){
	ST7567_FB_cls();
	initMenuMainPage();
}

/*
При отмене выходим на главный экран
*/
void woshingCancel(void){
	mainProcess = functionNull;
	ST7567_FB_cls();
	initMenuMainPage();
}


/*
Кнопки: Отмена, Начать
переназначаем кнопки и закпускаем процесс
*/
void woshingProcessStart(void){
	//перенастроили пункты меню
	menu_Nazad.next = &m_null;
	menu_Nazad.previous = &menu_Dalshe;
  //menu_Nazad.select = &menu_Recept;
	menu_Nazad.func = woshingCancel;
	label_Nazad.posX = 16; label_Nazad.posY = 50; label_Nazad.text = stringOtmena;
	
	menu_Dalshe.next =  &menu_Nazad;
	menu_Dalshe.previous = &m_null;
	menu_Dalshe.func = woshingSkip;
	label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringSkip;
	ST7567_FB_cls();
	ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)label_Woshing.text, NORMAL);
	ST7567_FB_display();
/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
	mainProcess = woshingProcess;
	MENU_Init(&menu_Nazad);
}

/*
Процесс мойки 
*/
void woshingProcess(void){
char _buf[8];
	uint8_t mm,ss;
	static uint64_t _tmr  = 0;
	
	if(deviceStatus.woshingTime != 0){
		if(millis() - _tmr > 1000){
			_tmr = millis();
			deviceStatus.woshingTime--;
			mm = deviceStatus.woshingTime / 60;//Столько минут осталось
			ss = deviceStatus.woshingTime % 60;//Столько секунд осталось
			sprintf(_buf, "%02d :%02d", mm ,ss);
			ST7567_FB_fillRect(48, 30,  32, 13, 0);//Сотрем старое значение
			ST7567_FB_printText(ST7567_FB_align_text(_buf, ALIGN_CENTR), 30, _buf, NORMAL);
			
		}
}
//если отсчет закончился переходим к режимк ополаскивания
	if(deviceStatus.woshingTime == 0){
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_2, NORMAL);

			//перенастроили пункты меню
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_Dalshe;
		//menu_Nazad.select = &menu_Recept;
	/*!*/	menu_Nazad.func = woshingCancel;
		label_Nazad.posX = 16; label_Nazad.posY = 50; label_Nazad.text = stringOtmena;
		
		menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = rinsingProcessStart;
		label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringBegin;

	/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
		mainProcess = functionNull;
		MENU_Init(&menu_Nazad);
			
	}
	
ST7567_FB_display();
	
}



/*
Пропускаем процесс мойки и переходим к процессу ополаскивания
*/
void woshingSkip(void){
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_2, NORMAL);

			//перенастроили пункты меню
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_Dalshe;
		//menu_Nazad.select = &menu_Recept;
	/*!*/	menu_Nazad.func = woshingCancel;
		label_Nazad.posX = 16; label_Nazad.posY = 50; label_Nazad.text = stringOtmena;
		
		menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = rinsingProcessStart;
		label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringBegin;

	/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
		mainProcess = functionNull;
		MENU_Init(&menu_Nazad);
}


/*
Экран запуска процесса полоскания.
Переходим к процессу дезинфекции
*/

void rinsingProcessStart(void){
	
		menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = rinsingProcessSkip;
		label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringSkip;
	
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingRinsing, NORMAL);
		ST7567_FB_display();
	mainProcess = rinsingProcess;
	MENU_Init(&menu_Nazad);
}


/*
Процесс полоскания/обеззараживания
*/
void rinsingProcess(void){
char _buf[8];
	uint8_t mm,ss;
	static uint64_t _tmr  = 0;
	
	if(deviceStatus.woshingTime != 0){
		if(millis() - _tmr > 1000){
			_tmr = millis();
			deviceStatus.woshingTime--;
			mm = deviceStatus.woshingTime / 60;//Столько минут осталось
			ss = deviceStatus.woshingTime % 60;//Столько секунд осталось
			sprintf(_buf, "%02d :%02d", mm ,ss);
			ST7567_FB_fillRect(48, 30,  32, 13, 0);//Сотрем старое значение
			ST7567_FB_printText(ST7567_FB_align_text(_buf, ALIGN_CENTR), 30, _buf, NORMAL);
			
		}
}
//если отсчет закончился переходим к режиму  дезинфекции
	if(deviceStatus.woshingTime == 0){
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_2, NORMAL);

			//перенастроили пункты меню
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_Dalshe;
		//menu_Nazad.select = &menu_Recept;
	/*!*/	menu_Nazad.func = woshingCancel;
		label_Nazad.posX = 20; label_Nazad.posY = 50; label_Nazad.text = stringOtmena;
		
		menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = disinfectionProcessStart;
		label_Dalshe.posX = 68; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringBegin;
	ST7567_FB_cls();
	ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_3, NORMAL);
	ST7567_FB_display();
	/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
		mainProcess = functionNull;
		MENU_Init(&menu_Nazad);
			
	}
	
}

/**/
void rinsingProcessSkip(void){
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_3, NORMAL);

			//перенастроили пункты меню
		
		menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = disinfectionProcessStart;
		label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringBegin;

	/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
		mainProcess = functionNull;
		MENU_Init(&menu_Nazad);
}
/**/
void disinfectionProcessStart(void){
	
			menu_Dalshe.next =  &menu_Nazad;
		menu_Dalshe.previous = &m_null;
	/*!*/	menu_Dalshe.func = disinfectionProcessSkip;
		label_Dalshe.posX = 65; label_Dalshe.posY = label_Nazad.posY;label_Dalshe.text = stringSkip;
	
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingDesinfektion, NORMAL);
/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
	mainProcess = disinfection;
	MENU_Init(&menu_Nazad);

}

/**/
void disinfectionProcessSkip(void){
	deviceStatus.woshingTime = 0;
	mainProcess = disinfection;
}

/**/
void disinfection(void){
char _buf[8];
	uint8_t mm,ss;
	static uint64_t _tmr  = 0;
	
	if(deviceStatus.woshingTime != 0){
		if(millis() - _tmr > 1000){
			_tmr = millis();
			deviceStatus.woshingTime--;
			mm = deviceStatus.woshingTime / 60;//Столько минут осталось
			ss = deviceStatus.woshingTime % 60;//Столько секунд осталось
			sprintf(_buf, "%02d :%02d", mm ,ss);
			ST7567_FB_fillRect(48, 30,  32, 13, 0);//Сотрем старое значение
			ST7567_FB_printText(ST7567_FB_align_text(_buf, ALIGN_CENTR), 30, _buf, NORMAL);
			
		}
}
	
//если отсчет закончился переходим в основное меню
	if(deviceStatus.woshingTime == 0){
		ST7567_FB_cls();
		ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingState_2, NORMAL);

			//перенастроили пункты меню
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &m_null;
		//menu_Nazad.select = &menu_Recept;
	/*!*/	menu_Nazad.func = woshingComplite;
		label_Nazad.posX = 60; label_Nazad.posY = 50; label_Nazad.text = stringOK;
		

	ST7567_FB_cls();
	ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, (char*)stringWoshingComplite, NORMAL);
	ST7567_FB_display();
	/*!*/	deviceStatus.woshingTime = woshingTime;//20*60;//20 мминут 
		mainProcess = functionNull;
		MENU_Init(&menu_Nazad);
			
	}
}

/**/
void woshingComplite(void){
	
woshingCancel();
}









