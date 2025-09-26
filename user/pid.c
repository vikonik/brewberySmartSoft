#include "pid.h"
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "allDefenition.h"
#include "delay.h"

PIDController_t pid;

uint64_t pidTimeoutCnt = 0;

uint8_t relay_state = 0;
float current_temperature = 20.0f; // Начальная температура

// Функция-заглушка для получения температуры
float get_temperature() {
//    // Имитация изменения температуры
//    static int counter = 0;
//    counter++;
//    
//    if (relay_state) {
//        // Нагрев - температура растет
//        current_temperature += 0.5f;
//        if (current_temperature > 100.0f) current_temperature = 100.0f;
//    } else {
//        // Охлаждение - температура падает
//        current_temperature -= 0.2f;
//        if (current_temperature < 15.0f) current_temperature = 15.0f;
//    }
//    
//    // Добавляем немного шума
//    float noise = ((rand() % 100) - 50) / 100.0f;
    return 22;
}

// Функция-заглушка для управления реле
void set_relay(uint8_t state) {
	if(!!state)
		PORT_SetBits(RELAY_PORT,HEAT_1);
	else
		PORT_ResetBits(RELAY_PORT,HEAT_1);
}

// Инициализация PID регулятора
void pid_init(PIDController_t* pid, float Kp, float Ki, float Kd, float setpoint) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->setpoint = setpoint;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output = 0.0f;
    pid->output_min = 0.0f;
    pid->output_max = 100.0f;
}

// Вычисление выходного значения PID
float pid_compute(PIDController_t* pid, float input) {
    float error = pid->setpoint - input;
    
    // Пропорциональная составляющая
    float proportional = pid->Kp * error;
    
    // Интегральная составляющая (антивиндアップ)
    pid->integral += error;
    // Ограничение интегральной составляющей
    if (pid->integral > 100.0f) pid->integral = 100.0f;
    if (pid->integral < -100.0f) pid->integral = -100.0f;
    float integral = pid->Ki * pid->integral;
    
    // Дифференциальная составляющая
    float derivative = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;
    
    // Суммируем все составляющие
    pid->output = proportional + integral + derivative;
    
    // Ограничиваем выходное значение
    if (pid->output > pid->output_max) pid->output = pid->output_max;
    if (pid->output < pid->output_min) pid->output = pid->output_min;
    
    return pid->output;
}

// Функция для работы ПИД регулятора с реле (цикл 1 минута)
void pid_relay_control(PIDController_t* pid) {
    float temperature = get_temperature();
    float pid_output = pid_compute(pid, temperature);
    
//    printf("Температура: %.1f°C, Setpoint: %.1f°C, PID output: %.1f%%", 
//           temperature, pid->setpoint, pid_output);
    
    // Управление реле на основе выходного значения PID
    // Используем гистерезис для предотвращения частых переключений
    static int counter = 0;
	
	if(millis() - pidTimeoutCnt > 1000){
		pidTimeoutCnt = millis();
    counter++;
    
    if (pid_output > 50.0f) {
        // Включаем реле если выход PID больше 50%
        set_relay(1);
//        printf(" - Нагрев\n");
    } else if (pid_output < 45.0f) {
        // Выключаем реле если выход PID меньше 45%
        set_relay(0);
//       printf(" - Охлаждение\n");
    } 
//		else {
//        // Зона гистерезиса - сохраняем текущее состояние
////        printf(" - Поддержание (%s)\n", relay_state ? "нагрев" : "охлаждение");
//    }
    
    // Логирование каждые 5 минут
//    if (counter % 5 == 0) {
//        printf("=== Статистика за последние 5 минут ===\n");
//        printf("Средняя температура: %.1f°C\n", temperature);
//        printf("Работа реле: %.1f%% времени\n", pid_output);
//        printf("=======================================\n");
//    }
	}
}
