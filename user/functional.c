//#include "functional.h"
//#include <stdio.h>
//#include <stdint.h>
//#include <stdbool.h>
////#include <unistd.h>
//#include "delay.h"
//#include "MT_10.h"
//#include "MPR121.h"
//#include "MDR32_inout.h"
//#include "allDefenition.h"
//#include "HAL.h"


//// Определения кнопок
//#define BUTTON_ON 0x0800
//#define BUTTON_TEMP 0x0400
//#define BUTTON_FERMENT 0x0200
//#define BUTTON_UP 0x0200
//#define BUTTON_FILTRATION 0x0100
//#define BUTTON_DOWN 0x0100
//#define BUTTON_SET_TIMER 0x0080
//#define BUTTON_SET_START_DELAY 0x0040
//#define BUTTON_SET_TIME 0x0020

//// Состояния системы
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

//extern uint8_t displayArray[];
//extern uint8_t digit[];


//// Глобальные переменные
//SystemState current_state = STATE_OFF;
//bool buttons_locked = false;
//bool heating_on = false;
//bool fermentation_on = false;
//bool filtration_on = false;
//bool timer_running = false;
//bool start_delay_running = false;

//int target_temperature = 0;
//int current_temperature = 0;
//int timer_value = 0;
//int start_delay_value = 0;

//uint16_t buttonState = 0;
//extern uint16_t allButtonsRAW;
//extern uint8_t buzerEn;



//// Функция-заглушка для получения состояния кнопок
//uint16_t get_buttons_state() {
//    // В реальной системе здесь будет чтение регистров GPIO
//    // Возвращаем 0 для имитации отсутствия нажатий
//		if(PIO_ReadInputDataBit(PORT_MPR_121, PIN_MPR_121_INT) == 0)	//Когда контроллер сенсорных кнопок замечает изменение состояния кнопок, он выставляет линию в 0, пока данные не вычитаны
//		{
//			//_delay_ms(10);
//			allButtonsRAW = mpr121_get_touch(MPR121_ADDRESS_BUTTONS);		
//			
//			//hotpanel.allButtons = ReorderButtons(allButtonsRAW);				//Перегруппировка кнопок			
//    buzerEn = 1;
//    delay_ms(200);
//    buzerEn = 0;;
//		}	
//    return allButtonsRAW;
//}

//// Функция для проверки нажатия конкретной кнопки
//bool is_button_pressed(uint16_t button_mask) {
//    static uint16_t prev_buttons = 0;
//    uint16_t current_buttons = get_buttons_state();
//    bool pressed = (current_buttons & button_mask) && !(prev_buttons & button_mask);
//    prev_buttons = current_buttons;
//    return pressed;
//}

//// Функция для проверки удержания кнопки в течение 3 секунд
//bool is_button_held(uint16_t button_mask, int milliseconds) {
//    if (get_buttons_state() & button_mask) {
//        delay_ms(milliseconds);
//        return (get_buttons_state() && button_mask) != 0;
//    }
//    return false;
//}

//// Функция для отображения на дисплее
//void display_message(const char* message) {
//    printf("Дисплей: %s\n", message);
//}

//// Функция для управления индикаторами
//void set_indicator(int indicator_num, bool state) {
//    printf("Индикатор %d: %s\n", indicator_num, state ? "ВКЛ" : "ВЫКЛ");
//}

//// Функция для управления ТЭН и насосом
//void set_heating(bool state) {
//    heating_on = state;
//    printf("ТЭН: %s\n", state ? "ВКЛ" : "ВЫКЛ");
//    set_indicator(3, state); // L3 - индикатор ТЭН
//}

//void set_pump(bool state) {
//    printf("Насос: %s\n", state ? "ВКЛ" : "ВЫКЛ");
//    set_indicator(2, state); // L2 - индикатор насоса
//}

//// Функция для работы со слайдером (заглушка)
//int get_slider_value() {
//    // В реальной системе здесь будет чтение значений с сенсорного слайдера
//    return 50; // возвращаем среднее значение
//}

//// Обработчик включения/выключения
//void handle_power_button() {
//    if (is_button_held(BUTTON_ON, 3000)) {
//        if (current_state == STATE_OFF) {
//            // Включение контроллера
//            current_state = STATE_IDLE;
// //           printf("Контроллер включен\n");
//            // Кратковременное включение всех индикаторов
//							displayArray[0] = digit[8];
//							displayArray[1] =	digit[8];
//							displayArray[2] =	digit[8];
//							displayArray[3] =	digit[8];
//							displayArray[4] =	digit[8];
//							displayArray[5] =	digit[8];
//							displayArray[6] =	digit[8];
//							displayArray[7] =	digit[8];
//							displayArray[8] =	digit[8];
//							displayArray[9] =	digit[8];
//							displayData(0, displayArray,10);
//            delay_ms(1000); // 0.5 секунды
//							displayArray[0] = digit[SPASE];
//							displayArray[1] =	digit[SPASE];
//							displayArray[2] =	digit[SPASE];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
//							delay_ms(1000);
//														displayArray[0] = digit[SPASE];
//							displayArray[1] =	digit[SPASE];
//							displayArray[2] =	digit[SPASE];
//							displayArray[3] =	digit[0];
//							displayArray[4] =	digit[0];
//							displayArray[5] =	digit[0];
//							displayArray[6] =	digit[0];
//							displayArray[7] =	digit[SIMBOL_t];
//							displayArray[8] =	digit[0];
//							displayArray[9] =	digit[0];
//							displayData(0, displayArray,10);
//							delay_ms(1000);							
//        } else {
//            // Выключение контроллера
//            current_state = STATE_OFF;
//            set_heating(false);
//            set_pump(false);
//  //          printf("Контроллер выключен\n");
//        }
//    }
//}

//// Обработчик блокировки кнопок
//void handle_buttons_lock() {
//    if (is_button_held(BUTTON_ON | BUTTON_TEMP, 3000)) {
//        buttons_locked = !buttons_locked;
//        if (buttons_locked) {
//							displayArray[0] = digit[SIMBOL_B];
//							displayArray[1] =	digit[SIMBOL_L];
//							displayArray[2] =	digit[SIMBOL_O];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
//            current_state = STATE_BUTTONS_LOCKED;
//  //          printf("Кнопки заблокированы\n");
//        } else {
//							displayArray[0] = digit[SPASE];
//							displayArray[1] =	digit[SPASE];
//							displayArray[2] =	digit[SPASE];
//							displayArray[3] =	digit[0];
//							displayArray[4] =	digit[0];
//							displayArray[5] =	digit[0];
//							displayArray[6] =	digit[0];
//							displayArray[7] =	digit[SIMBOL_t];
//							displayArray[8] =	digit[0];
//							displayArray[9] =	digit[0];
//							displayData(0, displayArray,10);
//            current_state = STATE_IDLE;
//   //         printf("Кнопки разблокированы\n");
//        }
//    }
//}

//// Обработчик установки температуры
//void handle_temperature_setting() {
//    if (is_button_held(BUTTON_TEMP, 3000)) {
//        if (current_state == STATE_IDLE || current_state == STATE_HEATING) {
//            current_state = STATE_TEMP_SETTING;
//					//Кратковременно включаем символ T
//							displayArray[0] = digit[SPASE];
//							displayArray[1] =	digit[SPASE];
//							displayArray[2] =	digit[SPASE];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SIMBOL_t];
//							displayArray[8] =	digit[deviceStatus.temperature/10];
//							displayArray[9] =	digit[deviceStatus.temperature % 10];
//							displayData(0, displayArray,10);
//					delay_ms(1000);

//            
//            // Установка температуры через слайдер
//            //target_temperature = get_slider_value();
// //           printf("Установлена температура: %d°C\n", target_temperature);
//  while(is_button_pressed(!BUTTON_TEMP)){        
//            // Точная регулировка кнопками
//            if (is_button_pressed(BUTTON_UP)) {
//                deviceStatus.temperature++;
// //               printf("Температура увеличена: %d°C\n", target_temperature);
//            }
//            if (is_button_pressed(BUTTON_DOWN)) {
//                deviceStatus.temperature--;
//  //              printf("Температура уменьшена: %d°C\n", target_temperature);
//            }
//            
//            // Запуск/остановка нагрева
//            if (is_button_held(BUTTON_TEMP, 3000)) {
//                if (current_state == STATE_HEATING) {
//                    set_heating(false);
//                    set_pump(false);
//                    current_state = STATE_IDLE;
//  //                  printf("Нагвык остановлен\n");
//                } else {
//                    set_heating(true);
//                    set_pump(true); // Включаем насос вместе с нагревом
//                    current_state = STATE_HEATING;
// //                   printf("Нагвык запущен\n");
//                }
//            }
//					}	
//						
//        }
//    }
//}

//// Обработчик ферментации
//void handle_fermentation() {
//    if (is_button_held(BUTTON_FERMENT, 3000)) {
//        fermentation_on = !fermentation_on;
//        if (fermentation_on) {
//							displayArray[0] = digit[SIMBOL_F];
//							displayArray[1] =	digit[SIMBOL_E];
//							displayArray[2] =	digit[SIMBOL_P];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
// //           printf("Ферментация включена\n");
//        } else {
// //           printf("Ферментация выключена\n");
//        }
//        
//        if (fermentation_on && filtration_on) {
//							displayArray[0] = digit[SIMBOL_F];
//							displayArray[1] =	digit[SIMBOL_F];
//							displayArray[2] =	digit[SIMBOL_P];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
//  //          printf("Ферментация и фильтрация активны\n");
//        }
//    }
//}

//// Обработчик фильтрации
//void handle_filtration() {
//    if (is_button_held(BUTTON_FILTRATION, 3000)) {
//        filtration_on = !filtration_on;
//        if (filtration_on) {
//							displayArray[0] = digit[SIMBOL_F];
//							displayArray[1] =	digit[SIMBOL_I];
//							displayArray[2] =	digit[SIMBOL_L];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
////            printf("Фильтрация включена\n");
//        } else {
////            printf("Фильтрация выключена\n");
//        }
//        
//        if (fermentation_on && filtration_on) {
//            	displayArray[0] = digit[SIMBOL_F];
//							displayArray[1] =	digit[SIMBOL_F];
//							displayArray[2] =	digit[SPASE];
//							displayArray[3] =	digit[SPASE];
//							displayArray[4] =	digit[SPASE];
//							displayArray[5] =	digit[SPASE];
//							displayArray[6] =	digit[SPASE];
//							displayArray[7] =	digit[SPASE];
//							displayArray[8] =	digit[SPASE];
//							displayArray[9] =	digit[SPASE];
//							displayData(0, displayArray,10);
////            printf("Ферментация и фильтрация активны\n");
//        }
//    }
//}

//// Обработчик отложенного старта
//void handle_start_delay() {
//    if (is_button_held(BUTTON_SET_START_DELAY, 3000)) {
//        current_state = STATE_START_DELAY_SETTING;
//        set_indicator(9, true); // Индикатор таймера
////        printf("Режим установки отложенного старта\n");
//        
//        // Установка времени через слайдер
// //       start_delay_value = get_slider_value();
// //       printf("Установлено время задержки: %d мин\n", start_delay_value);
//        
//        // Точная регулировка кнопками
//        if (is_button_pressed(BUTTON_UP)) {
//            start_delay_value++;
// //           printf("Время увеличено: %d мин\n", start_delay_value);
//        }
//        if (is_button_pressed(BUTTON_DOWN)) {
//            start_delay_value--;
////            printf("Время уменьшено: %d мин\n", start_delay_value);
//        }
//        
//        // Запуск/остановка отложенного старта
//        if (is_button_held(BUTTON_SET_START_DELAY, 3000)) {
//            start_delay_running = !start_delay_running;
//            if (start_delay_running) {
//                printf("Отложенный старт запущен\n");
//            } else {
//                printf("Отложенный старт остановлен\n");
//            }
//        }
//    }
//}

//// Обработчик таймера
//void handle_timer() {
//    if (is_button_held(BUTTON_SET_TIMER, 3000)) {
//        current_state = STATE_TIMER_SETTING;
//        set_indicator(9, true); // Индикатор таймера
//        printf("Режим установки таймера\n");
//        
//        // Установка времени через слайдер
//        timer_value = get_slider_value();
//        printf("Установлено время таймера: %d мин\n", timer_value);
//        
//        // Точная регулировка кнопками
//        if (is_button_pressed(BUTTON_UP)) {
//            timer_value++;
//            printf("Время увеличено: %d мин\n", timer_value);
//        }
//        if (is_button_pressed(BUTTON_DOWN)) {
//            timer_value--;
//            printf("Время уменьшено: %d мин\n", timer_value);
//        }
//        
//        // Запуск/остановка таймера
//        if (is_button_held(BUTTON_SET_TIMER, 3000)) {
//            timer_running = !timer_running;
//            if (timer_running) {
//                printf("Таймер запущен\n");
//            } else {
//                printf("Таймер остановлен\n");
//            }
//        }
//    }
//}

//// Основной цикл обработки
//void process_buttons(uint16_t btState) {
//	 buttonState   = btState;
//	
//    if (buttons_locked && current_state == STATE_BUTTONS_LOCKED) {
//        handle_buttons_lock(); // Только разблокировка
//        return;
//    }
//    
//    handle_power_button();
//    handle_buttons_lock();
//    handle_temperature_setting();
////    handle_fermentation();
////    handle_filtration();
////    handle_start_delay();
////    handle_timer();
//}

//// Функция для имитации работы системы
//int mainFunctional(uint16_t btState) {
////  buttonState   = btState;
////    
////    while (1) {
////        process_buttons();
////        delay_ms(100); // Проверка каждые 100 мс
////    }
////    
////    return 0;
//}






