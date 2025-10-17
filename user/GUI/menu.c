#include "stdint.h"
#include "stdio.h"
#include "menu.h"
#include "manifest.h"

#include "HAL.h"
//#include "ST7567_FB.h"
#include "font.h"
#include "microsoftSansSerif_8ptFont.h"
#include "manifest.h"

extern void (*submenuRegims[])(void);
extern char buf[];    
extern menu_t menuJurnal[];

const uint8_t charHeight = 13-1;//13 это высота шрифта

char menuBuf[32];

/*Кнопка OK одна и таже во всеж меню. Заполняем прямо так*/
const char label_OK[] = "OK";
const uint8_t OK_x = 110;
const uint8_t OK_y = (13-1) * 4;
const uint8_t OK_cursor_x = OK_x;
const uint8_t OK_cursor_y = OK_y + 13-1;
Item_t OK = {OK_x,OK_y,0,(Action_t)0,OK_cursor_x,OK_cursor_y,0,0, (char*)label_OK};


/*Установка времени и датв*/

// Глобальные переменные
// Пустой (несуществующий) элемент меню
menu_t m_null = {NULL, NULL, NULL, NULL, NULL, NULL};

// Текущий элемент меню
menu_t* m_curr = &m_null;



/***************** Общие команды ****************/
MenuText_t label_Nazad = {stringNazad, 1, 1};
MENU_ADD(menu_Nazad, m_null, m_null, m_null, NULL, NULL, &label_Nazad);
MenuText_t label_Dalshe = {stringDalshe, 1, 1};
MENU_ADD(menu_Dalshe, m_null, m_null, m_null, NULL, NULL, &label_Dalshe);


//MenuText_t label_Otmena = {stringOtmena, 1, 1};
//MENU_ADD(menu_Otmena m_null, m_null, m_null, NULL, NULL, &label_Otmena);

//MenuText_t label_OK = {stringOK, 1, 1};
//MENU_ADD(menu_OK, m_null, m_null, m_null, NULL, NULL, &label_OK);
/*
// Страница LED 0...2
MENU_ADD(m_p1i0, m_p1i1, m_null, m_null, LED_Handler, m_data[0], "    LED 0: %s");
MENU_ADD(m_p1i1, m_p1i2, m_p1i0, m_null, LED_Handler, m_data[1], "    LED 1: %s");
MENU_ADD(m_p1i2, m_p1i3, m_p1i1, m_null, LED_Handler, m_data[2], "    LED 2: %s");
MENU_ADD(m_p1i3, m_null, m_p1i2, m_p0i0, NULL,        NULL,      "    Back");

// Страница LED 3...5
MENU_ADD(m_p2i0, m_p2i1, m_null, m_null, LED_Handler, m_data[3], "    LED 3: %s");
MENU_ADD(m_p2i1, m_p2i2, m_p2i0, m_null, LED_Handler, m_data[4], "    LED 4: %s");
MENU_ADD(m_p2i2, m_p2i3, m_p2i1, m_null, LED_Handler, m_data[5], "    LED 5: %s");
MENU_ADD(m_p2i3, m_null, m_p2i2, m_p0i1, NULL,        NULL,      "    Back");

//Страница RGB
MENU_ADD(m_p3i0, m_p3i1, m_null, m_null, RGB_Handler, m_data[6],"    LED R: %s");
MENU_ADD(m_p3i1, m_p3i2, m_p3i0, m_null, RGB_Handler, m_data[7],"    LED G: %s");
MENU_ADD(m_p3i2, m_p3i3, m_p3i1, m_null, RGB_Handler, m_data[8],"    LED B: %s");
MENU_ADD(m_p3i3, m_null, m_p3i2, m_p0i2, NULL,        NULL,     "    Back");
*/


/*
Заполняем поля меню в соотсветствии с выбранным режимом Альтернатива макросу
*/
 void menuAdd(menu_t *name, menu_t *next, menu_t *previous, menu_t *select, void *func, void *data, MenuText_t *text){
     name->next = next;
     name->previous = previous;
     name->select =  select;
     
     name->func = *(fptr_t)func;
//     (void*)name->func = func;
     name->data = data;
     name->text = text;
 }

// Инициализация меню
void MENU_Init(menu_t* item)
{
  // Установка исходного элемента меню
  m_curr = item;

  // Отображение исходной страницы
  //  ST7567_FB_cls();
  MENU_PrintPage(m_curr);

  // Отображение указателя
  MENU_PrintPtr(m_curr);
}

// Перемещение по меню
void MENU_Move(menu_t* item)
{
  // Определение позиции выбранного элемента
  uint8_t item_pos = 0;
  for (menu_t *temp = item;
      (temp != &m_null) && (temp->previous != &m_null);
       temp = temp->previous, item_pos++);

  // Если выбранный элемент существует,
  // и расположен на допустимой позиции...
  if (item != &m_null && item_pos < MENU_ITEM_LIMIT) {

    // Отображение новой страницы меню,
    // если выбранный и текущий элементы не соседние
    if ((item != m_curr->next) && (item != m_curr->previous)) {
      MENU_PrintPage(item);
    }

    // Отображение указателя
    MENU_PrintPtr(item);

    // Запись выбранного элемента в текущий
    m_curr = item;
  }
}

// Выполнение функции элемента
void MENU_Exec(menu_t* item)
{
  // Выполнение функции элемента,
  // если она существует
  if (item->func != NULL) {
    item->func();
  }
}

// Отображение страницы меню
void MENU_PrintPage(menu_t* item)
{
   // ST7567_FB_cls();
    ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
    
    //Грабли для отображения состояния принудительного запуска
//    if(item == &menu_planirovshik || item == &menu_forsedLaunch || item == &menu_setEKO || item == &menu_rutine_nazad )
//        printForsetLaunch();
//  
    
      // Поиск начального элемента страницы
      for (uint8_t i = 0;
          (item->previous != &m_null) && (i < MENU_ITEM_LIMIT);
           item = item->previous, i++);

    //если строчек меню меньче чем максимальное число
    uint8_t max_item_pos = 0;
     for (menu_t *temp = item; temp->next != &m_null; temp = temp->next)
         max_item_pos++;

    max_item_pos++;//Грабли

    // Цикл отображения элементов страницы
    uint8_t tmpMENU_ITEM_LIMIT = MENU_ITEM_LIMIT;
    uint8_t tmpCnt = (tmpMENU_ITEM_LIMIT < max_item_pos) ? MENU_ITEM_LIMIT : max_item_pos;
    
    //Грабои для вывода прочерков в меню 
//      if(item == &menu_string_label_point_1 || item == &menu_string_label_point_2 ||item == &menu_string_label_point_3 ||item == &menu_string_label_point_4 /*|| item == &menu_point_nazad */){
//        printPoints();
//     }     
     
    for (uint8_t i = 0; (i < tmpCnt) ; i++) {
    // Отображение элемента
        ST7567_FB_printText(item->text->posX, item->text->posY, (char*)item->text->text, NORMAL); //(i ==0) ? INVERSE :NORMAL 

        // Переход к следующему элементу
        if (item != &m_null) {
          item = item->next;
        }
  }


    ST7567_FB_display();

}


/* 
Отображение указателя
*/
void MENU_PrintPtr(menu_t* item)
{
    MENU_PrintPage(item);
  // Стирание столбца указателей
//  for (uint8_t i = 0; i < MENU_ITEM_LIMIT; i++) {
//    //LCD_PrintSymbol(' ', MENU_PTR_POS, (MENU_STR_POS + i));
//  }

  // Определение позиции выбранного элемента
  uint8_t item_pos = 0;
  for (menu_t *temp = item;
      (temp != &m_null) && (temp->previous != &m_null);
       temp = temp->previous, item_pos++){};

  // Отображение указателя требуемую позицию
        ST7567_FB_setFont(&microsoftSansSerif_8ptFontInfo);
 //       ST7567_FB_printText(1, (cfont.CharacterHeight-1) * item_pos, (char*)item->text, INVERSE); //(i ==0) ? INVERSE :NORMAL 
        ST7567_FB_printText(item->text->posX, item->text->posY, (char*)item->text->text, INVERSE);
        ST7567_FB_display();
}

/**/
void MENU_Update(void){
  ST7567_FB_cls();
  MENU_PrintPage(m_curr);

  // Отображение указателя
  MENU_PrintPtr(m_curr);
}

/**************************** Перемещение по меню ***********************************************/

void menuNavigation(uint16_t buttonUp, uint16_t buttonDn, uint16_t buttonOK) {
    static uint16_t buttonOld = 0;
    uint16_t buttonCurrent = buttonUp | buttonDn | buttonOK;
    
    // Проверяем только фронт нажатия для каждой кнопки
    if (buttonUp && !(buttonOld & buttonUp)) {
        if(m_curr->previous != &m_null) {
            MENU_Move(m_curr->previous);
        } else {
            MENU_PrintPage(m_curr);
            MENU_PrintPtr(m_curr);
        }
    }

    if (buttonDn && !(buttonOld & buttonDn)) {
        if(m_curr->next != &m_null) {
            MENU_Move(m_curr->next);
        } else {
            MENU_PrintPage(m_curr);
            MENU_PrintPtr(m_curr);
        }
    }

    if (buttonOK && !(buttonOld & buttonOK)) {
        MENU_Exec(m_curr);
        MENU_Move(m_curr->select);
    }

    buttonOld = buttonCurrent;
}


/****************************Функции для работы с меню изменения значений************************/

/*
Передвигаем вперед курсор между позициями
Эту же функцию используем для инкримента целых чисел
uint8_t counter - Переменная в которой хранится счетчик
uint8_t maxVal- максимальное значение до которого считаем
*/
void numberSwitchCursorUp(uint8_t *counter, uint8_t minVal,  uint8_t maxVal){
// if(*counter == maxVal ) *counter = minVal;   
  (*counter)++;
  (*counter) %= maxVal;
  if((*counter) == 0) (*counter) = minVal;
}
/*
Передвигаем назад курсор между позициями
Эту же функцию используем для декримента целых чисел
uint8_t counter - Переменная в которой хранится счетчик
uint8_t minVal - значкение на которое переключать ся при переходе через ноль
uint8_t maxVal- максимальное значение до которого считаем

*/
void numberSwitchCursorDn(uint8_t *counter,uint8_t minVal,uint8_t maxVal){
if(*counter == minVal ) *counter = maxVal;
  (*counter)--;
  (*counter) %= maxVal;
}

/*
Устанавливаем курсор под для выбора позиции
*/
void setCursorPos(Item_t *cursor, uint8_t cursorLen){
    ST7567_FB_drawLineH(cursor->cursor_x, cursor->cursor_x + cursorLen, cursor->cursor_y, 2);//курсор OK, 11-ширина буквы
    ST7567_FB_display();
}

/*Устанавливаем курсор для изменения значения*/
void setChangeCursor(Item_t *cursor){
    setCursorPos(cursor, 11);//Отключили курсор
    sprintf(buf, "%02d", cursor->value);
    ST7567_FB_printText(cursor->x, cursor->y,buf, SWITCH);
    ST7567_FB_display();
}

/**/
void changeData(Item_t *item, uint8_t *encoderCounter, Inverse_t col ){
    item->value = *encoderCounter;
    sprintf(buf, "%02d", item->value);
    ST7567_FB_fillRect(item->x, item->y,11,cfont.CharacterHeight,col);//Стерли старую надпись
    ST7567_FB_printText(item->x, item->y,buf, col); //печатаем новый текст
    ST7567_FB_display();
}



/*Устанавливаем курсор для изменения значения*/
void setCursorChange(Item_t *cursor, Inverse_t vol){
    uint8_t width = 42;
    ST7567_FB_fillRect(cursor->x, cursor->y, width, 13, 0);
    ST7567_FB_printText(cursor->x, cursor->y,(char*)cursor->str, vol);
    ST7567_FB_display();
} 

/*Устанавливаем курсор для изменения числа*/
void setCursorChangeNumber(Item_t *cursor, Inverse_t vol){
    uint8_t width = 0;
    sprintf(buf, "%02d", cursor->value);
    width = ST7567_FB_strWidth(buf);
    ST7567_FB_fillRect(cursor->x, cursor->y, width, 13, 0);
    ST7567_FB_printText(cursor->x, cursor->y,buf, vol);
    ST7567_FB_display();
} 

/*
Действие выполняемое с пункутом меню
Может использоваться как для переключения между пунктами меню, так и для изменения значения пункта
*/
void actionMenuItem(void(*swichUp)(void), void(*svitchDn)(void), void(*switchOK)(void)){
//        Item_t* tmp =  (*AO_arr[cursorPos]);
//    if(!!encoderL){
//        encoderL = 0;
//        swichUp(); 
//    }

//    if(!!encoderR){
//        encoderR = 0;
//        svitchDn();
//    }

//    if(!!buttonPressed){
//        buttonPressed = 0;
//        switchOK();

//    }
}
/*
Изменение одного параметра
*/
void menuOneDataChange(){

    
}

/**
 * @brief Configures navigation menu buttons with specified functions
 * @param nazadFunc Function pointer for back button
 * @param dalsheFunc Function pointer for next button
 * @param nazadText Text for back button
 * @param dalsheText Text for next button
 * @param nazadPosX X position for back button text
 * @param dalshePosX X position for next button text
 */
void configureMenuButtons(void (*nazadFunc)(void), void (*dalsheFunc)(void), 
                               const char* nazadText, uint8_t nazadPosX,uint8_t nazadPosY,const char* dalsheText,
                               uint8_t dalshePosX, uint8_t dalshePosY)
{
    menu_Nazad.next = &m_null;
    menu_Nazad.previous = &menu_Dalshe;
    menu_Nazad.func = nazadFunc;
    label_Nazad.posX = nazadPosX;
    label_Nazad.posY = nazadPosY;
    label_Nazad.text = nazadText;
    
    menu_Dalshe.next = &menu_Nazad;
    menu_Dalshe.previous = &m_null;
    menu_Dalshe.func = dalsheFunc;
    label_Dalshe.posX = dalshePosX;
    label_Dalshe.posY = dalshePosY;
    label_Dalshe.text = dalsheText;
}




