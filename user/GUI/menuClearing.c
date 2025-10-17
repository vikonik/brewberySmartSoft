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

uint16_t woshingTime =  20*60;
extern void (*mainProcess)(void);

static const uint8_t firstStringH = 15;
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
void transitionToRinsingMenu(void);
void transitionToDisinfectionMenu(void);
void transitionToCompletionMenu(void);
void supportWoshingTemperature(uint8_t temperature);
		
MenuText_t label_WoshingState_1 = {stringWoshingState_1, 1, firstStringH * 1};
MenuText_t label_Woshing = {stringWoshing, 1, firstStringH * 1};



/**
 * @brief Displays time remaining in format MM:SS
 * @param timeRemaining Time in seconds to display
 * @param posY Y position for time display
 */
static void displayTimeRemaining(uint16_t timeRemaining, uint8_t posY){
    char timeBuf[8];
    uint8_t mm = timeRemaining / 60;
    uint8_t ss = timeRemaining % 60;
    
    sprintf(timeBuf, "%02d :%02d", mm, ss);
    ST7567_FB_fillRect(48, posY, 32, 13, 0);
    ST7567_FB_printText(ST7567_FB_align_text(timeBuf, ALIGN_CENTR), posY, timeBuf, NORMAL);
}

/**
 * @brief Initializes and displays the cleaning menu state 1
 */
void printMenuClearingState_1(void){
    configureMenuButtons(returnMenuClearingState_1, woshingProcessStart, 
                       stringOtmena,16,50, stringBegin, 80, 50);
    menu_Nazad.select = &m_null;
	
    ST7567_FB_cls();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
    ST7567_FB_printText(label_WoshingState_1.posX, label_WoshingState_1.posY, 
                       (char*)label_WoshingState_1.text, NORMAL);
    
    ST7567_FB_display();
	
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Returns to main menu screen
 */
void returnMenuClearingState_1(void){

    ST7567_FB_cls();
    initMenuMainPage();
}

/**
 * @brief Cancels washing process and returns to main menu
 */
void woshingCancel(void){
    mainProcess = functionNull;
	deviceStatus.isWoshing = 0;
	heatOff();
	nasosOff();
	collOff();
    ST7567_FB_cls();
    initMenuMainPage();
}

/**
 * @brief Starts washing process and configures menu buttons
 */
void woshingProcessStart(void){
    configureMenuButtons(woshingCancel, woshingSkip, 
                       stringOtmena,16, 50, stringSkip, 65, 50);
    
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)label_Woshing.text, NORMAL);
    ST7567_FB_display();
    
    deviceStatus.woshingTime = woshingTime;
    mainProcess = woshingProcess;
		deviceStatus.isWoshing = 1;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Handles washing process with countdown timer
 */
void woshingProcess(void){
    static uint64_t timer = 0;
    
    if(deviceStatus.woshingTime != 0)
    {
        if(millis() - timer > 1000)
        {
            timer = millis();
            deviceStatus.woshingTime--;
            displayTimeRemaining(deviceStatus.woshingTime, 30);
        }
				supportWoshingTemperature(65);
				nasosOn();
    }
    
    if(deviceStatus.woshingTime == 0)
    {
			nasosOff();
      supportWoshingTemperature(0);  
			transitionToRinsingMenu();
			
    }
    
    ST7567_FB_display();
}

/**
* @brief Переход к экрану меню Ополаскивание
 */
void transitionToRinsingMenu(void){
	supportWoshingTemperature(0);
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)stringWoshingState_2, NORMAL);
    
    configureMenuButtons(woshingCancel, rinsingProcessStart, 
                       stringOtmena,16,50, stringBegin, 65,50);
    
    deviceStatus.woshingTime = woshingTime;
    mainProcess = functionNull;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Skips washing process and transitions to rinsing
 */
void woshingSkip(void){
    transitionToRinsingMenu();
}

/**
* @brief Ополаскивание
 */
void rinsingProcessStart(void){
    configureMenuButtons(woshingCancel, rinsingProcessSkip, 
                       stringOtmena,16,50, stringSkip, 65,50);
    
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)stringWoshingRinsing, NORMAL);
    ST7567_FB_display();
    
    mainProcess = rinsingProcess;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Handles rinsing process with countdown timer
 */
void rinsingProcess(void){
    static uint64_t timer = 0;
    
    if(deviceStatus.woshingTime != 0)
    {
        if(millis() - timer > 1000)
        {
            timer = millis();
            deviceStatus.woshingTime--;
            displayTimeRemaining(deviceStatus.woshingTime, 30);
        }
				nasosOn();
				supportWoshingTemperature(0);
    }
    
    if(deviceStatus.woshingTime == 0)
    {
				nasosOff();
				supportWoshingTemperature(0);
        transitionToDisinfectionMenu();
    }
}

/**
 * @brief Transitions to disinfection menu screen
 */
void transitionToDisinfectionMenu(void){
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)stringWoshingState_3, NORMAL);
    
    configureMenuButtons(woshingCancel, disinfectionProcessStart, 
                       stringOtmena,20, 50, stringBegin, 68,50);
    
    deviceStatus.woshingTime = woshingTime;
    mainProcess = functionNull;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Skips rinsing process and transitions to disinfection
 */
void rinsingProcessSkip(void){
    transitionToDisinfectionMenu();
}

/**
 * @brief Starts disinfection process and configures menu buttons
 */
void disinfectionProcessStart(void){
    configureMenuButtons(woshingCancel, disinfectionProcessSkip, 
                       stringOtmena,16,50, stringSkip, 65, 50);
    
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)stringWoshingDesinfektion, NORMAL);
    
    deviceStatus.woshingTime = woshingTime;
    mainProcess = disinfection;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Skips disinfection process
 */
void disinfectionProcessSkip(void){
    deviceStatus.woshingTime = 0;
    mainProcess = disinfection;
}

/**
 * @brief Handles disinfection process with countdown timer
 */
void disinfection(void){
    static uint64_t timer = 0;
    
    if(deviceStatus.woshingTime != 0)
    {
				nasosOn();
				supportWoshingTemperature(0);
        if(millis() - timer > 1000)
        {
            timer = millis();
            deviceStatus.woshingTime--;
            displayTimeRemaining(deviceStatus.woshingTime, 30);
        }
    }
    
    if(deviceStatus.woshingTime == 0)
    {
			nasosOff();
      transitionToCompletionMenu();
    }
}

/**
 * @brief Transitions to completion menu screen
 */
void transitionToCompletionMenu(void){
    ST7567_FB_cls();
    ST7567_FB_printText(label_Woshing.posX, label_Woshing.posY, 
                       (char*)stringWoshingComplite, NORMAL);
    
    menu_Nazad.next = &m_null;
    menu_Nazad.previous = &m_null;
    menu_Nazad.func = woshingComplite;
    label_Nazad.posX = 60;
    label_Nazad.posY = 50;
    label_Nazad.text = stringOK;
    
    deviceStatus.woshingTime = woshingTime;
    mainProcess = functionNull;
    MENU_Init(&menu_Nazad);
}

/**
 * @brief Completes washing process and returns to main menu
 */
void woshingComplite(void){
    woshingCancel();
}

/**/
void supportWoshingTemperature(uint8_t temperature){
	if(deviceStatus.temperatureCurrent < temperature){
		heatOn();
	}
	else{
		heatOff();
}
}
