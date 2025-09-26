#ifndef FUNCTIONSL_H
#define FUNCTIONSL_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include <stdbool.h>

// Определения кнопок
//#define BUTTON_ON 0x0800
//#define BUTTON_TEMP 0x0400
//#define BUTTON_FERMENT 0x0200
//#define BUTTON_UP 0x0200
//#define BUTTON_FILTRATION 0x0100
//#define BUTTON_DOWN 0x0100
//#define BUTTON_SET_TIMER 0x0080
//#define BUTTON_SET_START_DELAY 0x0040
//#define BUTTON_SET_TIME 0x0020

// Состояния системы
//typedef enum {
//    STATE_OFF,
//    STATE_IDLE,
//    STATE_TEMP_SETTING,
//    STATE_FERMENTATION,
//    STATE_FILTRATION,
//    STATE_START_DELAY_SETTING,
//    STATE_TIMER_SETTING,
//    STATE_HEATING,
//    STATE_BUTTONS_LOCKED
//} SystemState;




// Функция-заглушка для получения состояния кнопок
uint16_t get_buttons_state(void) ;


// Функция для проверки нажатия конкретной кнопки
bool is_button_pressed(uint16_t button_mask) ;


// Функция для проверки удержания кнопки в течение 3 секунд
bool is_button_held(uint16_t button_mask, int milliseconds) ;

// Функция для отображения на дисплее
void display_message(const char* message) ;

// Функция для управления индикаторами
void set_indicator(int indicator_num, bool state) ;

// Функция для управления ТЭН и насосом
void set_heating(bool state) ;

void set_pump(bool state) ;

// Функция для работы со слайдером (заглушка)
int get_slider_value(void) ;

// Обработчик включения/выключения
void handle_power_button(void);


// Обработчик блокировки кнопок
void handle_buttons_lock(void) ;

// Обработчик установки температуры
void handle_temperature_setting(void);
// Обработчик ферментации
void handle_fermentation(void) ;

// Обработчик фильтрации
void handle_filtration(void) ;

// Обработчик отложенного старта
void handle_start_delay(void) ;

// Обработчик таймера
void handle_timer(void);

// Основной цикл обработки
void process_buttons(uint16_t btState);

// Функция для имитации работы системы
int mainFunctional(uint16_t btState) ;


#endif
