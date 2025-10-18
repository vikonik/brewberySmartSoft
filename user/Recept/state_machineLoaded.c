// state_machine.c
#include "state_machineLoaded.h"
#include "recipe_manager.h"
#include "ds18b20.h"
#include "allDefenition.h"
#include "delay.h"
#include "HAL.h"
#include "menuReceptLoaded.h"
#include "menu.h"
#include <stdio.h>
#include "menuReceptShablon.h"
//#include "relays.h"
//#include "display.h"
uint8_t stateMahineReceptTimerLoaded(void);
extern ShablonlControl_t manualControl[5];//Сюда копировать данные по времени и ремпературе из рецепта
// Глобальные переменные состояния
SystemStateLoaded_t current_stateLoaded = STATE_LOADED_IDLE;
ProcessStageLoaded_t current_stage_Loaded = STAGE_LOADED_NONE;
uint8_t current_mash_stage_Loaded = 0;
uint32_t stage_start_time_Loaded = 0;
uint32_t stage_remaining_time_Loaded = 0;

/**/
void StateMachine_Process_Start_Loaded(void){
	current_stateLoaded = STATE_LOADED_MASHING;
}

/**/
void StateMachine_Process_Stop_Loaded(void){
	current_stateLoaded = STATE_LOADED_IDLE;
	current_mash_stage_Loaded= 0;
	
}

// Основная функция обработки состояний
void StateMachine_Process_Loaded(void) {
    float current_temp = deviceStatus.temperatureCurrent;
    
    switch (current_stateLoaded) {
        case STATE_LOADED_IDLE:
            // Ожидание команды старта
//            heatOff();
//            nasosOff();
//            collOff();
            break;
            
        case STATE_LOADED_MASHING://Затирание
            StateMachine_ProcessMashingLoaded(current_temp);
            break;
        
				case STATE_LOADED_LAUTERING://Фильтрация
            StateMachine_ProcessLauteringLoaded(current_temp);
            break;
				
        case STATE_LOADED_BOILING://Кипячение
            StateMachine_ProcessBoilingLoaded(current_temp);
            break;
				
        case STATE_LOADED_COOLING://охлаждение
            StateMachine_ProcessCoolingLoaded(current_temp);
            break;
										
        case STATE_LOADED_FERMENTING://Ферментация
            StateMachine_ProcessFermentingLoaded(current_temp);
            break;

        default:
            break;
    }
}

// Обработка процесса затирания
void StateMachine_ProcessMashingLoaded(float current_temp) {
    MashStage* stage = &current_recipe.mash_stages[current_mash_stage_Loaded];
    float target_temp = (float)stage->temperature / 10.0f;

		    if (stateMahineReceptTimerLoaded()) {
        // Переход к следующей паузе
					current_mash_stage_Loaded++;
					if (current_mash_stage_Loaded >= current_recipe.mash_stages_count) {
							// Все паузы завершены - переход к фильтрации
							current_stateLoaded = STATE_LOADED_LAUTERING;
						beep(5);
						shablonSetFunctionFilnrationAutoResnartLoaded();
	//            current_stage_Loaded = STAGE_LAUTERING;
					} else {
						//Пеезаряжаем шаблон
						beep(5);
							shablonSetFunctionAutoResnartLoaded();
					}
    }
}

// Обработка процесса кипячения
void StateMachine_ProcessBoilingLoaded(float current_temp) {
    static uint8_t hop_additions_done = 0;
    uint32_t boil_time_remaining = stage_remaining_time_Loaded;
		
				  if (stateMahineReceptTimerLoaded()) {
        // Переход к следующей паузе
						beep(5);
							current_stateLoaded = STATE_LOADED_COOLING;
							shablonSetFunctionCoolingAutoResnartLoaded();
					}
}

// Обработка процесса ферментации
void StateMachine_ProcessFermentingLoaded(float current_temp) {
    float target_temp = (float)current_recipe.fermentation_temp / 10.0f;
  
					  if (stateMahineReceptTimerLoaded()) {
        // Переход к следующей паузе
							deviceStatus.pidEnable = 0;//Останавливаем 
							//shablonSetFunction();
							for(uint8_t i = 0; i < 10; i++){
								beep(1);
								delay_ms(100);
							}
							current_stateLoaded = STATE_LOADED_IDLE;
							
							//Написать функцию для завершения процесса
							beep(5);
							stopProcess();
						}
}

// Обработка процесса фильтрации (Lautering)
void StateMachine_ProcessLauteringLoaded(float current_temp) {

    
    // Насос работает постоянно для рециркуляции и промывки
    nasosOn();
    
	//Пока не выйднт таймер или температура не опустится ниже заданной
			    if (stateMahineReceptTimerLoaded()) {
						// Переход к следующмему циклу
beep(5);
							current_stateLoaded = STATE_LOADED_BOILING;
						shablonSetFunctionBoilingAutoResnartLoaded();
					}
    

}

// Обработка процесса охлаждения
void StateMachine_ProcessCoolingLoaded(float current_temp) {
    float target_temp = (float)current_recipe.fermentation_temp / 10.0f;
    

    			    if (stateMahineReceptTimerLoaded()  && deviceStatus.temperatureCurrent < current_recipe.fermentation_temp/10) {
						// Переход к следующмему циклу
beep(5);
							current_stateLoaded = STATE_LOADED_FERMENTING;
						shablonSetFunctionFermentingAutoResnartLoaded();
					}

}

/**/
uint8_t stateMahineReceptTimerLoaded(void){
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


