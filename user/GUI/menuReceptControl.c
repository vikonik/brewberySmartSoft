/*
Ход работы по рецпту
*/
#include "menuReceptControl.h"
#include "menuRecept.h"
#include "menuReceptShablon.h"
#include "mainPage.h"
#include "manifest.h"
#include "menu.h"
#include "recept.h"
#include "state_machine.h"

extern const Recipe_t pshenichnoe_beer, darck_beer, light_beer;
extern Recipe_t current_recipe;
extern SystemState_t current_state;
extern menu_t menu_Psenichnoe;
extern ShablonlControl_t receptControl[];
static const uint8_t firstStringH = 15;

void printMenuReceptControl(void);
	
/*
Переконфигурируем отображение меню для работы с рецептом
*/
void startRecepteProcess(void){
		menu_ShablonNazad.func = printMenuReceptControl;
	
	    //ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_ShablonNazad.next = &menu_ShablonSkip;
		menu_ShablonNazad.previous = &menu_ShablonTime;
		menu_ShablonNazad.select = &menu_Psenichnoe;
		menu_ShablonNazad.func = allMenuTextClear;
		menu_ShablonNazad.text->text = stringNazad;
		menu_ShablonNazad.text->posY = firstStringH + charHeight8pt*3;
		menu_ShablonNazad.text->posX = 1;
	
	//Добавляем пункт пропустиить
		menu_ShablonSkip.next = &menu_ShablonState;
		menu_ShablonSkip.previous = &menu_ShablonNazad;
	
	//Переопределяем пункт меню menu_ShablonState
	menu_ShablonState.previous = &menu_ShablonSkip;
	menu_ShablonState.func = shablonSetFunctionAuto;//Переназначаем используемую функцию
//printShablon(receptControl, &menu_ShablonTempersture, &menu_ShablonTime, &label_ShablonTemperature, &label_ShablonTimer);
	//Загружаем рецепт из памяти
	switch(*m_curr->data){
		case 0:
			current_recipe = pshenichnoe_beer;
		break;
		case 1:
			current_recipe = darck_beer;
		break;
		case 2:
			current_recipe = light_beer;	
		break;
		
	}
	
	//Выводим измененое меню на экран 
	printShablonAuto((Recipe_t*)&current_recipe, &current_state);

 
} 

/**/
void printMenuReceptControl(void){
    allMenuTextClear();
    //ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_ShablonNazad.next = &menu_ShablonSkip;
		menu_ShablonNazad.previous = &menu_ShablonTime;
		menu_ShablonNazad.select = &m_null;
		menu_ShablonNazad.func = allMenuTextClear;
		menu_ShablonNazad.text->text = stringNazad;
		menu_ShablonNazad.text->posY = firstStringH + charHeight8pt*3;
		menu_ShablonNazad.text->posX = 1;
	
	//Добавляем пункт пропустиить
		menu_ShablonSkip.next = &menu_ShablonState;
		menu_ShablonSkip.previous = &menu_ShablonNazad;
	
	
	//Переопределяем пункт меню menu_ShablonState
	menu_ShablonState.previous = &menu_ShablonSkip;
	
	
    MENU_Init(&menu_ShablonTempersture);
}
