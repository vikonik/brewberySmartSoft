#ifndef _STATE_MACHINE_LOADED_H
#define _STATE_MACHINE_LOADED_H	
#include <stdint.h>
// Основные состояния системы
typedef enum {
    STATE_LOADED_IDLE = 0,          // Ожидание
    STATE_LOADED_MASHING,           // Затирание
    STATE_LOADED_LAUTERING,         // Фильтрация и промывка зерна
    STATE_LOADED_BOILING,           // Кипячение
    STATE_LOADED_COOLING,           // Охлаждение
    STATE_LOADED_FERMENTING,        // Ферментация
    STATE_LOADED_CLEANING,          // Очистка
    STATE_LOADED_PAUSED,            // Пауза
    STATE_LOADED_ERROR,             // Ошибка
    STATE_LOADED_COMPLETED          // Процесс завершен
} SystemStateLoaded_t;

// Детальные стадии внутри основных состояний
typedef enum {
    STAGE_LOADED_NONE = 0,
    
    // Стадии затирания (MASHING)
    STAGE_LOADED_MASH_START,        // Начало затирания
    STAGE_LOADED_MASH_PROTEIN_REST, // Белковая пауза
    STAGE_LOADED_MASH_SACCHARIFICATION, // Осахаривание
    STAGE_LOADED_MASH_MASH_OUT,     // Мэш-аут
    
    // Стадии кипячения (BOILING)  
    STAGE_LOADED_BOIL_START,        // Начало кипячения
    STAGE_LOADED_BOIL_HOP_ADDITION, // Добавление хмеля
    
    // Стадии ферментации (FERMENTING)
    STAGE_LOADED_FERMENT_PRIMARY,   // Первичная ферментация
    STAGE_LOADED_FERMENT_SECONDARY, // Вторичная ферментация
    STAGE_LOADED_FERMENT_CONDITIONING, // Дображивание
    
    // Стадии очистки (CLEANING)
    STAGE_LOADED_CLEAN_RINSE,       // Ополаскивание
    STAGE_LOADED_CLEAN_WASH,        // Мойка
    STAGE_LOADED_CLEAN_SANITIZE     // Дезинфекция
} ProcessStageLoaded_t;

// Глобальные переменные состояния
//extern SystemState_t current_state;
//extern ProcessStage_t current_stage;
extern uint8_t current_mash_stage_Loaded;

void StateMachine_Process_Start_Loaded(void);
void StateMachine_Process_Stop_Loaded(void);

// Основная функция обработки состояний
void StateMachine_Process_Loaded(void); 

// Обработка процесса затирания
void StateMachine_ProcessMashingLoaded(float current_temp) ;

// Обработка процесса кипячения
void StateMachine_ProcessBoilingLoaded(float current_temp);

// Обработка процесса ферментации
void StateMachine_ProcessFermentingLoaded(float current_temp) ;
void StateMachine_ProcessLauteringLoaded(float current_temp);
void StateMachine_ProcessCoolingLoaded(float current_temp);

void switchStadge(void);

#endif
