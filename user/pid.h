#ifndef _PID_H
#define _PID_H

#include <stdint.h>

// Структура для PID регулятора
typedef struct {
    float Kp;           // Пропорциональный коэффициент
    float Ki;           // Интегральный коэффициент  
    float Kd;           // Дифференциальный коэффициент
    float setpoint;     // Заданная температура
    float integral;     // Интегральная составляющая
    float prev_error;   // Предыдущая ошибка
    float output;       // Выходное значение PID (-100% до +100%)
    float output_min;   // Минимальное выходное значение
    float output_max;   // Максимальное выходное значение
    float integral_max; // Максимальное значение интеграла
    uint32_t last_time; // Временная метка последнего вычисления
    uint8_t heating_active;  // Состояние нагревателя
    uint8_t cooling_active;  // Состояние охладителя
    float deadband;     // Мертвая зона (гистерезис)
} PIDController_t;

extern PIDController_t pid;

// Прототипы функций
void pid_Off(void);
void pid_init(PIDController_t* pid, float Kp, float Ki, float Kd, float setpoint);
float pid_compute(PIDController_t* pid, float input, uint32_t current_time);
void pid_relay_control(PIDController_t* pid);
void pid_set_setpoint(PIDController_t* pid, float setpoint);
void pid_set_deadband(PIDController_t* pid, float deadband);
float myFabs(float data);

#endif
