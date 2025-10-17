#ifndef _RECEPT_MANIGER_H
#define _RECEPT_MANIGER_H	
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup
#include "recept.h"
extern Recipe_t current_recipe;
// CRC-16/MODBUS полином: 0x8005 (x^16 + x^15 + x^2 + 1)
uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length);

// Вспомогательная функция для получения адреса страницы
uint32_t GetRecipePageAddress(uint8_t sector, uint8_t page_index); 

uint8_t Recipe_SaveToFlash(Recipe_t* new_recipe, uint8_t index); 

// Wear leveling - перенос всех рецептов в альтернативный сектор с обновлением
uint8_t Recipe_SaveWithWearLeveling(Recipe_t* new_recipe, uint8_t index);

// Чтение рецепта из Flash
uint8_t Recipe_ReadFromFlash(Recipe_t* recipe, uint8_t index); 

// Получение количества сохраненных рецептов
uint8_t Recipe_GetCount(void); 

// Инициализация системы хранения рецептов
void Recipe_StorageInit(void);
// Удаление рецепта
uint8_t Recipe_DeleteFromFlash(uint8_t index);



// Загрузка рецепта из FLASH памяти
uint8_t Recipe_LoadFromFlash(uint8_t recipe_index); 

// Получение списка рецептов
uint8_t Recipe_GetList(char recipe_names[][32], uint8_t max_count); 

// Альтернативная версия GetList только для чтения имен (быстрее, но без проверки CRC)
uint8_t Recipe_GetListFast(char recipe_names[][32], uint8_t max_count); 

// Получение информации о рецепте без загрузки полной структуры
uint8_t Recipe_GetInfo(uint8_t recipe_index, char* name, uint16_t* original_gravity);

uint8_t Recipe_GetCount(void);
uint8_t Recipe_GetNames(char names[][32], uint8_t max_names);

void preSetRecepteToFlash(void);
#endif
