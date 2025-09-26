
#include "brew_control.h"
#include "allDefenition.h"
#include "delay.h"
// Заглушки (реализуете сами)
 float read_temp(void){};
 void heater_on(void){};
 void heater_off(void){};
 bool heater_state(void){};
 void pump_on(void){};
 void pump_off(void){};
 bool pump_state(void){};
 void alarm_on(const char *msg){};
 void alarm_off(void){};
 bool alarm_active(void){};
// uint32_t millis(void){return 22;}

// -------------------------
// Настройки по умолчанию
// -------------------------
static BrewConfig config = {
    .hysteresis_C = 0.3f,
    .alarm_high_C = 30.0f,
    .pump_after_heat_ms = 600000,  // 10 минут
    .pump_on_time_ms = 60000,      // 1 минута
    .pump_off_time_ms = 180000     // 3 минуты
};

// -------------------------
// Локальные переменные
// -------------------------
static BrewMode mode = MODE_IDLE;
static const FermentStep *profile = NULL;
static uint8_t N_STEPS = 0;
static uint8_t current_step = 0;
static float T_set_current = 0;

// Время начала шага ферментации
static uint32_t step_start_time_ms = 0;

// Фильтрация
static uint32_t filtration_end_time_ms = 0;
static uint32_t pump_timer_ms = 0;
static bool pump_running = false;

// Ферментация: удержание работы насоса после нагрева
static uint32_t pump_hold_timer_ms = 0;
static bool was_heater_on = false; // Флаг предыдущего состояния нагревателя

// -------------------------
// Вспомогательные функции
// -------------------------

/**
 * @brief Обновление состояния термостата
 * @param[in] T    Текущая температура
 * @param[in] Tset Заданная температура
 * @bref Включает/выключает нагрев в зависимости от гистерезиса
 */
static void thermostat_update(float T, float Tset) {
    if (T < (Tset - config.hysteresis_C)) {
        if (!heater_state()) heater_on();
    } else if (T > (Tset + config.hysteresis_C)) {
        if (heater_state()) heater_off();
    }
}

/**
 * @brief Управление насосом в режиме ферментации
 * @bref Насос работает при нагреве и продолжает работать после выключения нагрева
 *       в течение заданного времени (pump_after_heat_ms)
 */
static void fermentation_pump_update(void) {
    uint32_t now = millis();

    if (alarm_active()) {
        if (!pump_state()) pump_on();
        return;
    }

    if (heater_state()) {
        // Нагреватель работает -> насос должен работать
        if (!pump_state()) pump_on();
        was_heater_on = true;
    } else {
        if (was_heater_on) {
            // Нагреватель только что выключился -> запускаем таймер
            pump_hold_timer_ms = now;
            was_heater_on = false;
        }
        
        // Проверяем, не истекло ли время работы насоса после нагрева
        if ((now - pump_hold_timer_ms) < config.pump_after_heat_ms) {
            if (!pump_state()) pump_on();
        } else {
            if (pump_state()) pump_off();
        }
    }
}

/**
 * @brief Управление насосом в режиме фильтрации
 * @bref Циклическое включение/выключение насоса по таймеру
 */
static void filtration_pump_update(void) {
    uint32_t now = millis();
    
    if (alarm_active()) {
        if (!pump_state()) pump_on();
        return;
    }

    if (pump_running) {
        if ((now - pump_timer_ms) >= config.pump_on_time_ms) {
            pump_off();
            pump_running = false;
            pump_timer_ms = now;
        }
    } else {
        if ((now - pump_timer_ms) >= config.pump_off_time_ms) {
            pump_on();
            pump_running = true;
            pump_timer_ms = now;
        }
    }
}

/**
 * @brief Проверка безопасности системы
 * @param[in] T Текущая температура
 * @bref При превышении температуры включает аварию и аварийное охлаждение
 */
static void safety_check(float T) {
    if (T > config.alarm_high_C) {
        alarm_on("Overtemperature!");
        heater_off();
        pump_on(); // Аварийное охлаждение
    }
}

// -------------------------
// API
// -------------------------

void brew_init(void) {
    mode = MODE_IDLE;
    heater_off();
    pump_off();
    alarm_off();
    was_heater_on = false;
}

void brew_set_config(const BrewConfig *cfg) {
    if (cfg) config = *cfg;
}

/*
Запустить при включении ферментации
аргументы:
указатель на структуру с циклами ферментации
количество циклов
*/
void fermentation_start(const FermentStep *steps, uint8_t n) {
    if (steps == NULL || n == 0) return;
    
    mode = MODE_FERMENTATION;
    profile = steps;
    N_STEPS = n;
    current_step = 0;
    step_start_time_ms = millis();
    T_set_current = steps[0].target_C;
    pump_hold_timer_ms = step_start_time_ms;
    was_heater_on = false;
}

/*
Запустить при включении фильтрации
аргумент - длительность фильтрации в секундах
*/
void filtration_start(uint32_t duration_sec) {
    mode = MODE_FILTRATION;
    uint32_t now = millis();
    filtration_end_time_ms = now + duration_sec * 1000;
    pump_on();
    heater_off();
    pump_timer_ms = now;
    pump_running = true;
}

void brew_stop(void) {
    mode = MODE_IDLE;
    heater_off();
    pump_off();
    alarm_off();
    was_heater_on = false;
}

BrewMode brew_mode(void) {
    return mode;
}

void brew_tick(void) {
    float T = read_temp();
    uint32_t now = millis();

    switch (mode) {
    case MODE_IDLE:
        break;

    case MODE_FERMENTATION: {
        if (current_step >= N_STEPS) {
            brew_stop();
            break;
        }
        
        const FermentStep *st = &profile[current_step];

        // Вычисляем прошедшее время в часах
        float elapsed_h = (now - step_start_time_ms) / 3600000.0f;
        float max_delta = st->ramp_C_per_h * elapsed_h;

        if (st->ramp_C_per_h > 0) {
            // Плавное изменение температуры
            float diff = st->target_C - T_set_current;
            if (diff > 0) T_set_current += (diff > max_delta ? max_delta : diff);
            else if (diff < 0) T_set_current -= ((-diff) > max_delta ? max_delta : (-diff));
        } else {
            // Мгновенный переход к целевой температуре
            T_set_current = st->target_C;
        }

        thermostat_update(T, T_set_current);
        fermentation_pump_update();

        // Проверяем завершение текущего шага
        if ((now - step_start_time_ms) >= st->hold_hours * 3600000UL) {
            current_step++;
            if (current_step < N_STEPS) {
                step_start_time_ms = now;
                T_set_current = profile[current_step].target_C;
            } else {
                brew_stop();
            }
        }

        safety_check(T);
        break;
    }

    case MODE_FILTRATION:
        thermostat_update(T, 76.0f); // Фильтрация при 76°C
        filtration_pump_update();
        if (now >= filtration_end_time_ms) brew_stop();
        safety_check(T);
        break;
    }
}
