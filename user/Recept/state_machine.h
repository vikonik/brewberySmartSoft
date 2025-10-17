#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H	
#include <stdint.h>
// Основные состояния системы
typedef enum {
    STATE_IDLE = 0,          // Ожидание
    STATE_MASHING,           // Затирание
    STATE_LAUTERING,         // Фильтрация и промывка зерна
    STATE_BOILING,           // Кипячение
    STATE_COOLING,           // Охлаждение
    STATE_FERMENTING,        // Ферментация
    STATE_CLEANING,          // Очистка
    STATE_PAUSED,            // Пауза
    STATE_ERROR,             // Ошибка
    STATE_COMPLETED          // Процесс завершен
} SystemState_t;

// Детальные стадии внутри основных состояний
typedef enum {
    STAGE_NONE = 0,
    
    // Стадии затирания (MASHING)
    STAGE_MASH_START,        // Начало затирания
    STAGE_MASH_PROTEIN_REST, // Белковая пауза
    STAGE_MASH_SACCHARIFICATION, // Осахаривание
    STAGE_MASH_MASH_OUT,     // Мэш-аут
    
    // Стадии кипячения (BOILING)  
    STAGE_BOIL_START,        // Начало кипячения
    STAGE_BOIL_HOP_ADDITION, // Добавление хмеля
    
    // Стадии ферментации (FERMENTING)
    STAGE_FERMENT_PRIMARY,   // Первичная ферментация
    STAGE_FERMENT_SECONDARY, // Вторичная ферментация
    STAGE_FERMENT_CONDITIONING, // Дображивание
    
    // Стадии очистки (CLEANING)
    STAGE_CLEAN_RINSE,       // Ополаскивание
    STAGE_CLEAN_WASH,        // Мойка
    STAGE_CLEAN_SANITIZE     // Дезинфекция
} ProcessStage_t;

// Глобальные переменные состояния
extern SystemState_t current_state;
extern ProcessStage_t current_stage;
extern uint8_t current_mash_stage;

void StateMachine_Process_Start(void);
void StateMachine_Process_Stop(void);

// Основная функция обработки состояний
void StateMachine_Process(void); 

// Обработка процесса затирания
void StateMachine_ProcessMashing(float current_temp) ;

// Обработка процесса кипячения
void StateMachine_ProcessBoiling(float current_temp);

// Обработка процесса ферментации
void StateMachine_ProcessFermenting(float current_temp) ;
void StateMachine_ProcessLautering(float current_temp);
void StateMachine_ProcessCooling(float current_temp);

void switchStadge(void);

#endif
