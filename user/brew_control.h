
#ifndef BREW_CONTROL_H
#define BREW_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#define NULL 0

/**
 * @brief Режимы работы пивоварни
 */
typedef enum {
    MODE_IDLE,         ///< Система выключена, нагрев и насос отключены
    MODE_FERMENTATION, ///< Режим ферментации по профилю
    MODE_FILTRATION    ///< Режим фильтрации (отжим)
} BrewMode;

/**
 * @brief Структура шага ферментации
 */
typedef struct {
    float target_C;        ///< Целевая температура, °C
    float ramp_C_per_h;    ///< Скорость изменения температуры, °C/ч (>0 - нагрев, <0 - охлаждение, 0 - мгновенный переход)
    uint32_t hold_hours;   ///< Длительность шага, часы
} FermentStep;

/**
 * @brief Конфигурация контроллера
 */
typedef struct {
    float hysteresis_C;         ///< Гистерезис термостата, °C
    float alarm_high_C;         ///< Температура срабатывания аварии, °C
    uint32_t pump_after_heat_ms;///< Время работы насоса после нагрева, мс
    uint32_t pump_on_time_ms;   ///< Время ВКЛ насоса в режиме фильтрации, мс
    uint32_t pump_off_time_ms;  ///< Время ВЫКЛ насоса в режиме фильтрации, мс
} BrewConfig;

/**
 * @brief Инициализация системы пивоварни
 * @bref Выключает нагрев, насос и сбрасывает аварию
 */
void brew_init(void);

/**
 * @brief Установка конфигурации
 * @param[in] cfg Указатель на структуру конфигурации
 * @bref Копирует переданную конфигурацию во внутреннюю структуру
 */
void brew_set_config(const BrewConfig *cfg);

/**
 * @brief Запуск режима ферментации
 * @param[in] steps Массив шагов ферментации
 * @param[in] n     Количество шагов в массиве
 * @bref Инициализирует переменные и запускает процесс ферментации
 */
void fermentation_start(const FermentStep *steps, uint8_t n);

/**
 * @brief Запуск режима фильтрации
 * @param[in] duration_sec Длительность фильтрации в секундах
 * @bref Включает насос и запускает таймер завершения процесса
 */
void filtration_start(uint32_t duration_sec);

/**
 * @brief Основной цикл управления
 * @bref Должен вызываться периодически в основном цикле программы
 *       Обновляет температуру, управляет нагревом и насосом
 */
void brew_tick(void);

/**
 * @brief Остановка процесса пивоварни
 * @bref Переводит систему в режим IDLE, отключает нагрев и насос
 */
void brew_stop(void);

/**
 * @brief Получение текущего режима работы
 * @return Текущий режим работы из перечисления BrewMode
 */
BrewMode brew_mode(void);

#endif // BREW_CONTROL_H
