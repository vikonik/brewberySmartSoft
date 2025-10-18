// state_machine.c
#include "state_machine.h"
#include "recipe_manager.h"
#include "ds18b20.h"
#include "allDefenition.h"
#include "delay.h"
#include "HAL.h"
#include "menuReceptShablon.h"
#include <stdio.h>
//#include "relays.h"
//#include "display.h"
uint8_t stateMahineReceptTimer(void);
extern ShablonlControl_t manualControl[5];//Сюда копировать данные по времени и ремпературе из рецепта
// Глобальные переменные состояния
SystemState_t current_state = STATE_IDLE;
ProcessStage_t current_stage = STAGE_NONE;
uint8_t current_mash_stage = 0;
uint32_t stage_start_time = 0;
uint32_t stage_remaining_time = 0;

/**/
void StateMachine_Process_Start(void){
	current_state = STATE_MASHING;
}

/**/
void StateMachine_Process_Stop(void){
	current_state = STATE_IDLE;
	current_mash_stage= 0;
	
}

// Основная функция обработки состояний
void StateMachine_Process(void) {
    float current_temp = deviceStatus.temperatureCurrent;
    
    switch (current_state) {
        case STATE_IDLE:
            // Ожидание команды старта
//            heatOff();
//            nasosOff();
//            collOff();
            break;
            
        case STATE_MASHING://Затирание
            StateMachine_ProcessMashing(current_temp);
            break;
        
				case STATE_LAUTERING://Фильтрация
            StateMachine_ProcessLautering(current_temp);
            break;
				
        case STATE_BOILING://Кипячение
            StateMachine_ProcessBoiling(current_temp);
            break;
				
        case STATE_COOLING://охлаждение
            StateMachine_ProcessCooling(current_temp);
            break;
										
        case STATE_FERMENTING://Ферментация
            StateMachine_ProcessFermenting(current_temp);
            break;

        default:
            break;
    }
}

// Обработка процесса затирания
void StateMachine_ProcessMashing(float current_temp) {
    MashStage* stage = &current_recipe.mash_stages[current_mash_stage];
    float target_temp = (float)stage->temperature / 10.0f;

		    if (stateMahineReceptTimer()) {
        // Переход к следующей паузе
					current_mash_stage++;
					if (current_mash_stage >= current_recipe.mash_stages_count) {
							// Все паузы завершены - переход к фильтрации
							current_state = STATE_LAUTERING;
						beep(5);
						shablonSetFunctionFilnrationAutoResnart();
	//            current_stage = STAGE_LAUTERING;
					} else {
						//Пеезаряжаем шаблон
						beep(5);
							shablonSetFunctionAutoResnart();
					}
    }
}

// Обработка процесса кипячения
void StateMachine_ProcessBoiling(float current_temp) {
    static uint8_t hop_additions_done = 0;
    uint32_t boil_time_remaining = stage_remaining_time;
		
				  if (stateMahineReceptTimer()) {
        // Переход к следующей паузе
						beep(5);
							current_state = STATE_COOLING;
							shablonSetFunctionCoolingAutoResnart();
					}
}

// Обработка процесса ферментации
void StateMachine_ProcessFermenting(float current_temp) {
    float target_temp = (float)current_recipe.fermentation_temp / 10.0f;
  
					  if (stateMahineReceptTimer()) {
        // Переход к следующей паузе
							deviceStatus.pidEnable = 0;//Останавливаем 
							//shablonSetFunction();
							for(uint8_t i = 0; i < 10; i++){
								beep(1);
								delay_ms(100);
							}
							current_state = STATE_IDLE;
							
							//Написать функцию для завершения процесса
							beep(5);
							stopProcess();
						}
}

// Обработка процесса фильтрации (Lautering)
void StateMachine_ProcessLautering(float current_temp) {

    
    // Насос работает постоянно для рециркуляции и промывки
    nasosOn();
    
	//Пока не выйднт таймер или температура не опустится ниже заданной
			    if (stateMahineReceptTimer()) {
						// Переход к следующмему циклу
beep(5);
							current_state = STATE_BOILING;
						shablonSetFunctionBoilingAutoResnart();
					}
    

}

// Обработка процесса охлаждения
void StateMachine_ProcessCooling(float current_temp) {
    float target_temp = (float)current_recipe.fermentation_temp / 10.0f;
    

    			    if (stateMahineReceptTimer()  && deviceStatus.temperatureCurrent < current_recipe.fermentation_temp/10) {
						// Переход к следующмему циклу
beep(5);
							current_state = STATE_FERMENTING;
						shablonSetFunctionFermentingAutoResnart();
					}

}

/**/
uint8_t stateMahineReceptTimer(void){
	  static char bufTemp[8];
		static char bufTime[8];
		static uint64_t shablonExecutePeriod = 0;
		static uint64_t curentTic = 0;
extern MenuText_t label_ShablonCurrentTime;	
extern MenuText_t label_ShablonCurentTemperature;		
	curentTic = millis();
	if(curentTic - shablonExecutePeriod > 1000){
		
		//Выводим измеренную температуру
    sprintf(bufTemp, "%02.1f", deviceStatus.temperatureCurrent);
		label_ShablonCurentTemperature.text = bufTemp;

// Уменьшаем секунды
    if (deviceStatus.manualControlCurrentData.targetTimer_s > 0) {
        deviceStatus.manualControlCurrentData.targetTimer_s--;
    } else {
        // Если секунды закончились, уменьшаем минуты
        deviceStatus.manualControlCurrentData.targetTimer_s = 59;
        
        if (deviceStatus.manualControlCurrentData.targetTimer_m > 0) {
            deviceStatus.manualControlCurrentData.targetTimer_m--;
        } else {
            // Если минуты закончились, уменьшаем часы
            deviceStatus.manualControlCurrentData.targetTimer_m = 59;
            
            if (deviceStatus.manualControlCurrentData.targetTimer_h > 0) {
                deviceStatus.manualControlCurrentData.targetTimer_h--;
            } else {
                // Таймер завершен
                deviceStatus.manualControlCurrentData.targetTimer_h = 0;
                deviceStatus.manualControlCurrentData.targetTimer_m = 0;
                deviceStatus.manualControlCurrentData.targetTimer_s = 0;
                
                // Здесь можно вызвать callback или установить флаг события
                //timer_finished_callback();
							deviceStatus.pidEnable = 0;//Останавливаем 
							
							
							//shablonSetFunction();

								beep(5);

							
            }
        }
    }
    
		
    sprintf(bufTime,"%03d:%02d:%02d",	deviceStatus.manualControlCurrentData.targetTimer_h, 
																			deviceStatus.manualControlCurrentData.targetTimer_m, 
																			deviceStatus.manualControlCurrentData.targetTimer_s);
		label_ShablonCurrentTime.text = bufTime;
		shablonPrintCurrentData();	
shablonExecutePeriod = curentTic;		
	}

	if(deviceStatus.manualControlCurrentData.targetTimer_h == 0 &&
     deviceStatus.manualControlCurrentData.targetTimer_m == 0 &&
     deviceStatus.manualControlCurrentData.targetTimer_s == 0){
		 return 1;
		 }
		 return 0;
	
}

/*
Переклюсение стадий приготовления
*/
void switchStadge(void){
if(current_state == STATE_IDLE)
	current_state++;

 switch (current_state) {
           
        case STATE_MASHING://Затирание
							current_mash_stage++;
							if (current_mash_stage >= current_recipe.mash_stages_count) {
									// Все паузы завершены - переход к фильтрации
									current_state = STATE_LAUTERING;
								shablonSetFunctionFilnrationAutoResnart();
							} 
							else {
								//Пеезаряжаем шаблон
									shablonSetFunctionAutoResnart();
							}
            break;
        
				case STATE_LAUTERING://Фильтрация
 							current_state = STATE_BOILING;
							shablonSetFunctionBoilingAutoResnart();           
            break;
				
        case STATE_BOILING://Кипячение
            	current_state = STATE_COOLING;
							shablonSetFunctionCoolingAutoResnart();
            break;
				
        case STATE_COOLING://охлаждение
            	current_state = STATE_FERMENTING;
							shablonSetFunctionFermentingAutoResnart();
            break;
										
        case STATE_FERMENTING://Ферментация
							current_state = STATE_IDLE;
            break;

        default:
            break;
    }
}

