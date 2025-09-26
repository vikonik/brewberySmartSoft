#ifndef _PID_H
#define _PID_H

// Структура для PID регулятора
typedef struct {
    float Kp;           // Пропорциональный коэффициент
    float Ki;           // Интегральный коэффициент  
    float Kd;           // Дифференциальный коэффициент
    float setpoint;     // Заданная температура
    float integral;     // Интегральная составляющая
    float prev_error;   // Предыдущая ошибка
    float output;       // Выходное значение PID
    float output_min;   // Минимальное выходное значение
    float output_max;   // Максимальное выходное значение
} PIDController_t;
extern PIDController_t pid;

void pid_init(PIDController_t* pid, float Kp, float Ki, float Kd, float setpoint) ;
void pid_relay_control(PIDController_t* pid);

#endif
