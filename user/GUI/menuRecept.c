/*
Выбираем какой рецепт будем готовить

*/
#include "menuRecept.h"
#include "allDefenition.h"
#include "ST7567_FB.h"
#include <stdio.h>
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"
#include "menu.h"
#include "mainPage.h"
#include "recipe_manager.h"
#include "menuReceptShablon.h"
#include "menuReceptControl.h"
#include "menuReceptLoaded.h"

void printMenuReceptInt(void);
void printMenuReceptLoaded(void);
void printReseptProces(void);
static void returnToMainPage(void);

static const uint8_t firstStringH = 15;

MenuText_t label_ReceptIns = {stringReceptIns, 1, firstStringH};
MenuText_t label_ReceptLoaded = {stringReceptLoaded, 1, firstStringH + charHeight8pt};

MENU_ADD(menu_ReceptIns,		menu_ReceptLoaded,  	m_null,             m_null,             printMenuReceptInt,   NULL,      &label_ReceptIns);
MENU_ADD(menu_ReceptLoaded,	menu_Nazad,  					menu_ReceptIns,     m_null,             printMenuReceptLoaded,   NULL,      &label_ReceptLoaded);
//MENU_ADD(menu_Nazad,				menu_Temnoe,  	m_null,             m_null,             NULL,   NULL,      &label_Psenichnoe);


MenuText_t label_Psenichnoe = {stringPsenichnoe, 1, firstStringH};
MenuText_t label_Temnoe = {stringTemnoe, 1, firstStringH + charHeight8pt};
MenuText_t label_Svetloe = {stringSvetloe, 1, firstStringH + charHeight8pt*2};

char insBeerNumber[3] = {0,1,2};
MENU_ADD(menu_Psenichnoe,	menu_Temnoe,  	m_null,             m_null,     startRecepteProcess,   			&insBeerNumber[0],      &label_Psenichnoe);
MENU_ADD(menu_Temnoe ,   	menu_Svetloe,   menu_Psenichnoe,    m_null,     startRecepteProcess,   			&insBeerNumber[1],      &label_Temnoe);
MENU_ADD(menu_Svetloe,   	menu_Nazad,     menu_Temnoe,      	m_null,     startRecepteProcess,   			&insBeerNumber[2],      &label_Svetloe);

/****************** Меню для загруженных рецептов *****************/
MenuText_t label_Recept_s_Loaded[3];
MenuText_t label_ReseptDell = {stringReseptDell, 64, firstStringH + charHeight8pt*3};
uint8_t menu_ReceptLoaded_Data[3];//Здесь храним индекс рецепта для удаления
MENU_ADD(menu_ReceptLoaded_1,	m_null,  	m_null,             m_null,       startRecepteProcessLoaded,  &menu_ReceptLoaded_Data[0],      &label_Recept_s_Loaded[0]);
MENU_ADD(menu_ReceptLoaded_2,	m_null,  	m_null,             m_null,       startRecepteProcessLoaded,  &menu_ReceptLoaded_Data[1],      &label_Recept_s_Loaded[1]);
MENU_ADD(menu_ReceptLoaded_3,	m_null,  	m_null,             m_null,       startRecepteProcessLoaded,  &menu_ReceptLoaded_Data[2],      &label_Recept_s_Loaded[2]);

//MENU_ADD(menu_ReceptDelet,		m_null,  	menu_Nazad,         m_null,             NULL,   NULL,      &label_ReseptDell);

menu_t *menu_ReceptLoaded_arr[3] = {
	&menu_ReceptLoaded_1,
	&menu_ReceptLoaded_2,
	&menu_ReceptLoaded_3
};


/******************* Шаблон для отображеиня хода приготовления *************/
ShablonlControl_t shablonlControl;
void printReseptProces(void){
		//printShablon(&shablonlControl, &menu_ShablonTempersture, &menu_ShablonTime, &label_ShablonTemperature, &label_ShablonTimer);
		//printShablon(&shablonlControl, &menu_ShablonTempersture, &menu_ShablonTime);

}

/******************* Управление рецептом ***************************/
/**
 * @brief Меню выбора встоенных рецептов или загруженных
 */
void printMenuReceptMain(void){
    allMenuTextClear();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_ReceptLoaded;
		menu_Nazad.select = &menu_Recept;
		menu_Nazad.func = allMenuTextClear;
		menu_Nazad.text->text = stringNazad;
		menu_Nazad.text->posY = firstStringH + charHeight8pt*2;
		menu_Nazad.text->posX = 1;
    MENU_Init(&menu_ReceptIns);
}

/**/
void printMenuReceptInt(void){
    allMenuTextClear();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &menu_Svetloe;
		menu_Nazad.select = &menu_ReceptIns;
		menu_Nazad.func = printMenuReceptMain;
		menu_Nazad.text->text = stringNazad;
		menu_Nazad.text->posY = firstStringH + charHeight8pt*3;
		menu_Nazad.text->posX = 1;
    MENU_Init(&menu_Psenichnoe);
}
/**/
void printMenuReceptLoaded(void){
		allMenuTextClear();
	//Читаем рецепты из Флэш и выводим на дисплей
	    // Инициализируем систему рецептов
    Recipe_StorageInit();
		uint8_t count = Recipe_GetCount();
	//Это одинаково в любом случае
	menu_Nazad.select = &m_null;
	menu_Nazad.func = printMenuReceptMain;
	menu_Nazad.text->text = stringNazad;
	menu_Nazad.text->posY = firstStringH + charHeight8pt*3;
	menu_Nazad.text->posX = 1;
	
	if(!count){//Если рецептов нет то выводим сообщение об их отсутствии
		menu_ReceptLoaded_1.text->text = stringNoRecepte;
		menu_ReceptLoaded_1.text->posX = 1;
		menu_ReceptLoaded_1.text->posY = firstStringH;
		
		ST7567_FB_printText(menu_ReceptLoaded_1.text->posX, menu_ReceptLoaded_1.text->posY, (char*)menu_ReceptLoaded_1.text->text, NORMAL);
	//	ST7567_FB_display();
		menu_Nazad.next = &m_null;
		menu_Nazad.previous = &m_null;
		menu_Nazad.select = &m_null;
		menu_Nazad.func = printMenuReceptMain;
		menu_Nazad.text->text = stringNazad;
		menu_Nazad.text->posY = firstStringH + charHeight8pt*3;
		menu_Nazad.text->posX = 1;
    MENU_Init(&menu_Nazad);
		return;
	}
	// Получаем названия рецептов
    static char recipe_names[3][32];  // Буфер для 3 названий
    uint8_t found = Recipe_GetNames(recipe_names, 10);
	uint8_t menuCnt = 0;
	if(count > 3)
		count = 3;
	for(menuCnt = 0; menuCnt < count; menuCnt++){
		if(menuCnt == 0)
			menu_ReceptLoaded_arr[menuCnt]->previous = &m_null;
		else
			menu_ReceptLoaded_arr[menuCnt]->previous = menu_ReceptLoaded_arr[menuCnt-1];
		
		if(menuCnt < count-1)//если не последняя строка		
			menu_ReceptLoaded_arr[menuCnt]->next = menu_ReceptLoaded_arr[menuCnt+1];
		else
			menu_ReceptLoaded_arr[menuCnt]->next = &menu_Nazad;
		
		menu_ReceptLoaded_arr[menuCnt]->select = &m_null;
	//	menu_ReceptLoaded_arr[menuCnt]->func = NULL;	
		menu_ReceptLoaded_arr[menuCnt]->text->text = recipe_names[menuCnt];
		menu_ReceptLoaded_arr[menuCnt]->text->posX = 1;
		menu_ReceptLoaded_arr[menuCnt]->text->posY = firstStringH + charHeight8pt*menuCnt;
		menu_ReceptLoaded_Data[menuCnt] = menuCnt;
		menu_ReceptLoaded_arr[menuCnt]->data = (char*)&menu_ReceptLoaded_Data[menuCnt];
	}
	
	//Дописываем недостающие пункты
	menu_Nazad.previous = menu_ReceptLoaded_arr[count-1];
	menu_Nazad.next = &m_null;//&menu_ReceptDelet;
	
	
  MENU_Init(&menu_Nazad);
}
/**/
static void returnToMainPage(void){

    ST7567_FB_cls();
    initMenuMainPage();
}
