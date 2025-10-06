/*
Вызываем в цикле pid_relay_control(&pid);
*/
#include "pid.h"
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "allDefenition.h"
#include "delay.h"

PIDController_t pid;
const uint16_t pumpWorkDelay = 5;//Задержка на включение
const uint16_t pumpWorkPeriod = pumpWorkDelay + 120; //Время работы помпы
const uint16_t pumpStopPeriod = 60;//Время простоя помпы
uint64_t pidTimeoutCnt = 0;
uint64_t pumpTimeoutCnt = 0;
uint8_t relay_state = 0;
uint16_t pumpCnt = 0;;

float current_temperature = 20.0f; // Начальная температура

// Функция-заглушка для получения температуры
float get_temperature() {
    return deviceStatus.temperatureCurrent;
}

// Функция-заглушка для управления реле


void set_heater(uint8_t state) {
    if(state)
        PORT_SetBits(RELAY_PORT, HEAT_1);  // Включить нагрев
    else
        PORT_ResetBits(RELAY_PORT, HEAT_1); // Выключить нагрев
}

void set_cooler(uint8_t state) {
    if(state)
        PORT_SetBits(RELAY_PORT, COOL_1);  // Включить охлаждение
    else
        PORT_ResetBits(RELAY_PORT, COOL_1); // Выключить охлаждение
}

void set_pump(uint8_t state) {
    if(state)
        PORT_SetBits(RELAY_PORT, MIXER);  // Включить охлаждение
    else
        PORT_ResetBits(RELAY_PORT, MIXER); // Выключить охлаждение
}

/**/
void pid_Off(void){
	deviceStatus.pidEnable = 0;
	pumpCnt = 0;
	set_heater(0);
	set_cooler(0);
	set_pump(0);
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
    pid->output_min = -100.0f;  // -100% (полное охлаждение)
    pid->output_max = 100.0f;   // +100% (полный нагрев)
    pid->heating_active = 0;
    pid->cooling_active = 0;
    pid->last_time = millis();
    pid->deadband = 2.0f;       // Мертвая зона ±2°C по умолчанию
    
    // Выключить все реле при инициализации
    set_heater(0);
    set_cooler(0);

}

// Установка мертвой зоны (гистерезиса)
void pid_set_deadband(PIDController_t* pid, float deadband) {
    pid->deadband = fabsf(deadband);
}

// Установка новой уставки
void pid_set_setpoint(PIDController_t* pid, float setpoint) {
    pid->setpoint = setpoint;
}

// Вычисление выходного значения PID
float pid_compute(PIDController_t* pid, float input, uint32_t current_time) {
    // Вычисление времени с последнего вызова
    uint32_t dt_ms = current_time - pid->last_time;
    float dt = dt_ms / 1000.0f;  // Переводим в секунды
    
    // Защита от первого вызова и больших интервалов
    if(dt <= 0 || dt > 1.0f) dt = 0.1f;
    
    // Ошибка регулирования
    float error = pid->setpoint - input;
    
    // Применяем мертвую зону к ошибке
    if(fabsf(error) <= pid->deadband) {
        error = 0.0f;
    }
    
    // Пропорциональная составляющая
    float proportional = pid->Kp * error;
    
    // Интегральная составляющая (с учетом времени)
    pid->integral += error * dt;
    
    // Антивинд-up интеграла
    float integral_max = 50.0f / (pid->Ki != 0 ? pid->Ki : 1.0f);
    if(pid->integral > integral_max) pid->integral = integral_max;
    if(pid->integral < -integral_max) pid->integral = -integral_max;
    
    float integral = pid->Ki * pid->integral;
    
    // Дифференциальная составляющая (с учетом времени)
    float derivative = pid->Kd * (error - pid->prev_error) / dt;
    
    pid->prev_error = error;
    pid->last_time = current_time;
    
    // Суммируем все составляющие
    pid->output = proportional + integral + derivative;
    
    // Ограничиваем выходное значение
    if(pid->output > pid->output_max) pid->output = pid->output_max;
    if(pid->output < pid->output_min) pid->output = pid->output_min;
    
    return pid->output;
}

/*
Функция для работы ПИД регулятора с реле
*/
void pid_relay_control(PIDController_t* pid) {
    if(millis() - pidTimeoutCnt > 1000) {
        pidTimeoutCnt = millis();
        
        float temperature = get_temperature();
        float pid_output = pid_compute(pid, temperature, millis());
        
        // Статические переменные для ШИМ-счетчиков
        static uint32_t pwm_counter = 0;
        static uint8_t last_heat_state = 0;
        static uint8_t last_cool_state = 0;
        
        pwm_counter++;
        if(pwm_counter >= 10) pwm_counter = 0;  // 10-секундный ШИМ цикл
        
        // Определяем текущую мощность в процентах
        float heat_power = 0.0f;
        float cool_power = 0.0f;
        
        if(pid_output > 0) {
            // Нужен нагрев
            heat_power = pid_output;  // 0-100%
            cool_power = 0.0f;
        } else if(pid_output < 0) {
            // Нужно охлаждение
            heat_power = 0.0f;
            cool_power = -pid_output; // 0-100% (положительное значение)
        } else {
            // Поддержание температуры - оба выключены
            heat_power = 0.0f;
            cool_power = 0.0f;
        }
        
        // Управление нагревателем с ШИМ
        if(heat_power > (pwm_counter * 10)) {
            set_heater(1);
            pid->heating_active = 1;
        } else {
            set_heater(0);
            pid->heating_active = 0;
        }
        
        // Управление охладителем с ШИМ
        if(cool_power > (pwm_counter * 10)) {
            set_cooler(1);
            pid->cooling_active = 1;
        } else {
            set_cooler(0);
            pid->cooling_active = 0;
        }
        
        // Защита от одновременной работы нагрева и охлаждения
        if(pid->heating_active && pid->cooling_active) {
            // Аварийная ситуация - выключаем все
            set_heater(0);
            set_cooler(0);
            pid->heating_active = 0;
            pid->cooling_active = 0;
        }
        
        // Логирование (можно убрать в релизе)
        /*
        printf("Temp: %.1f°C, Set: %.1f°C, PID: %.1f%%, Heat: %d%%, Cool: %d%%\n", 
               temperature, pid->setpoint, pid_output, 
               (int)heat_power, (int)cool_power);
        */
    }
		
		if(millis() - pumpTimeoutCnt > 1000){
			pumpTimeoutCnt = millis();
			pumpCnt++;
			
			if(pumpCnt < pumpWorkDelay){//Задержка на включение
				set_pump(0);
			}
			else if(pumpCnt <= pumpWorkPeriod){
				set_pump(1);
			}
			else if(pumpCnt >= pumpStopPeriod){
				set_pump(0);
				pumpCnt = 0;
			}
			
			
		}
}
