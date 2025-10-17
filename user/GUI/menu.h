#ifndef __MENU_H
#define __MENU_H

#include "ST7567_FB.h"

extern void (*encoderFunction)(void);
extern int encoderL;
extern int encoderR;
extern uint8_t buttonPressed;
extern uint8_t cursorPos;       //Переменная объявлена в Encoder.c
extern uint8_t encoderCounter;  //Переменная объявлена в Encoder.c




//extern const uint8_t charHeight;
// Типы данных
// Указатель на функцию
typedef void (*fptr_t)(void);



// Структура элементов меню
//typedef struct {
//  void       *next;      // Следующий элемент
//  void       *previous;  // Предыдущий элемент
//  void       *select;    // Подменю или действие
//  fptr_t      func;      // Функция элемента
//  char       *data;      // Данные элемента
//  const char *text;      // Текст элемента

//} menu_t;
//typedef struct{

//}HelpText_t;


typedef struct {
  const char *text;     // Текст элемента
  uint8_t posX;         //Указываем позицию  Х куда печатать символ
  uint8_t posY;         //Указываем позицию  Y куда печатать символ
}MenuText_t;


extern MenuText_t label_Nazad;
extern MenuText_t label_Dalshe;


typedef struct {
  void       *next;      // Следующий элемент
  void       *previous;  // Предыдущий элемент
  void       *select;    // Подменю или действие
  fptr_t      func;      // Функция элемента
  char       *data;      // Данные элемента
  MenuText_t *text;      // Текст элемента
}menu_t;


/************************** Пункты всего меню ***************************/
extern menu_t menu_Recept, menu_Clearing, menu_Setting;
extern menu_t menu_Dalshe, menu_Nazad, menu_Otmena, menu_OK;



// Кол-во элементов на странице
#define MENU_ITEM_LIMIT 8

// Константы отображения
#define MENU_STR_POS 3  // Позиция первой строки меню (ординат)
#define MENU_PTR_POS 2  // Позиция указателя (абсцисс)

// Макрос добавления элемента меню
#define MENU_ADD(name, next, previous, select, func, data, text)  \
  extern menu_t next;                                             \
  extern menu_t previous;                                         \
  extern menu_t select;                                           \
  menu_t name = {(void*)&next, (void*)&previous, (void*)&select,  \
                 (fptr_t)func, (void*)data,      (MenuText_t*)text};    \

// Кроссмодульные переменные
extern menu_t* m_curr;
extern menu_t m_null; 



// Прототипы функций
void menuAdd(menu_t *name, menu_t *next, menu_t *previous, menu_t *select, void *func, void *data, MenuText_t *text);                 
void MENU_Init(menu_t* item);
void MENU_Move(menu_t* item);
void MENU_Exec(menu_t* item);
void MENU_PrintPage(menu_t* m_item);
void MENU_PrintPtr(menu_t* item);
void MENU_Update(void);         

/**/
//Состояние каждого элемента меню, режим работы или режим изменения
typedef enum{
    WORK,
    CHANGE
}Action_t;

//информация о символе
typedef struct{
    uint8_t x;//положение символа на экране
    uint8_t y;  
    uint8_t value;//значение
    Action_t regim;//редактирование/работа
    uint8_t cursor_x;//положение курсора
    uint8_t cursor_y;
    uint8_t value_max;//значения которые может принимать символ
    uint8_t value_min;
    const char* str;//Указатель на строку
}Item_t;

extern Item_t OK; 

//формат времени и двты для отображения на экране
typedef struct{
    Item_t hh;
    Item_t razdelitel_hhmm;
    Item_t mm;
    Item_t ss;
    Item_t DD;
    Item_t razdelitel_DDMM;
    Item_t MM;
    Item_t razdelitel_MMYY;
    Item_t YY;
    Item_t WD;//день недели
    Item_t OK;
}Calendar_t;
extern Calendar_t calendar;


/**/

/*Точки установки аремени работы системы*/
typedef struct{
    uint8_t pointEn; //Маркер активности точки
    Item_t hhStart; //часы и минуты начала 
    Item_t mmStart;
    Item_t hhStop; //часы и минуты конца 
    Item_t mmStop;

//    uint8_t posStartX;  //Координаты надписи
//    uint8_t posStartY;
//    uint8_t posStopX;   //Координаты надписи
//    uint8_t posStopY;   
}Point_t;
extern Point_t points[7][4];

void numberSwitchCursorUp(uint8_t *counter, uint8_t minVal,  uint8_t maxVal);
void numberSwitchCursorDn(uint8_t *counter,uint8_t minVal,uint8_t maxVal);
void setCursorPos(Item_t *cursor, uint8_t cursorLen);
void setCursorChange(Item_t *cursor, Inverse_t vol);
void setCursorChangeNumber(Item_t *cursor, Inverse_t vol);
void setChangeCursor(Item_t *cursor);
void changeData(Item_t *item, uint8_t *encoderCounter, Inverse_t col );
void actionMenuItem( void(*swichUp)(void), void(*svitchDn)(void), void(*switchOK)(void));
void initSubmenuPlanirovchic(void);
void menuNavigation(uint16_t buttonUp, uint16_t buttonDn, uint16_t buttonOK);
void configureMenuButtons(void (*nazadFunc)(void), void (*dalsheFunc)(void), 
                               const char* nazadText, uint8_t nazadPosX,uint8_t nazadPosY,const char* dalsheText,
                               uint8_t dalshePosX, uint8_t dalshePosY);
#endif
