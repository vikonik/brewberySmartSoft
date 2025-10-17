#include "recipe_manager.h"
#include "recept.h"
#include "w25qxx.h"
#include <stdio.h>
#include <stddef.h> //для offsetof

#define RECIPES_SECTOR_2     2  // Сектор 2
#define RECIPES_SECTOR_3     3  // Сектор 3
#define SECTOR_SIZE          4096
#define PAGE_SIZE            256
#define MAX_RECIPES_PER_SECTOR  (SECTOR_SIZE / PAGE_SIZE)  // 16 рецептов на сектор
#define MAX_RECIPES          (MAX_RECIPES_PER_SECTOR * 2)  // 32 рецепта всего

// Глобальные переменные для управления
static uint8_t active_sector = RECIPES_SECTOR_2;
static uint8_t recipe_count = 0;
// Глобальные переменные
Recipe_t current_recipe;
uint8_t current_recipe_index = 0;
uint8_t total_recipes = 0;

// CRC-16/MODBUS полином: 0x8005 (x^16 + x^15 + x^2 + 1)
uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length) {
    uint16_t crc = 0xFFFF;  // Initial value
    uint32_t i, j;
    
    if (data == NULL || length == 0) {
        return 0;
    }
    
    for (i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;  // 0xA001 - это обратный полином 0x8005
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

// Вспомогательная функция для получения адреса страницы
uint32_t GetRecipePageAddress(uint8_t sector, uint8_t page_index) {
   // return W25qxx_SectorToPage(sector) + (page_index * (PAGE_SIZE / PAGE_SIZE)); // page_index т.к. каждая страница = 256 байт
	return W25qxx_SectorToPage(sector) + page_index;
}

uint8_t Recipe_SaveToFlash(Recipe_t* new_recipe, uint8_t index) {
    if (index >= MAX_RECIPES) return 0;
    
    // Рассчитываем CRC перед сохранением
    new_recipe->crc16 = CRC16_Calculate((uint8_t*)new_recipe, sizeof(Recipe_t) - 2);
    
    // Определяем целевой сектор и индекс страницы
    uint8_t target_sector, page_index;
    if (index < MAX_RECIPES_PER_SECTOR) {
        target_sector = RECIPES_SECTOR_2;
        page_index = index;
    } else {
        target_sector = RECIPES_SECTOR_3;
        page_index = index - MAX_RECIPES_PER_SECTOR;
    }
    
    uint32_t page_address = GetRecipePageAddress(target_sector, page_index);
    
    // Проверяем, пустая ли страница
    if (!W25qxx_IsEmptyPage(page_address, 0, sizeof(Recipe_t))) {
        // Страница не пустая - используем wear leveling
        return Recipe_SaveWithWearLeveling(new_recipe, index);
    }
    
    // Страница пустая - просто записываем
    W25qxx_WritePage((uint8_t*)new_recipe, page_address, 0, sizeof(Recipe_t));
    
    // Обновляем счетчик рецептов
    if (index >= recipe_count) {
        recipe_count = index + 1;
    }
    
    return 1;
}

/*
Wear leveling - перенос всех рецептов в альтернативный сектор с обновлением
*/
uint8_t Recipe_SaveWithWearLeveling(Recipe_t* new_recipe, uint8_t index) {
    uint8_t source_sector, target_sector;
    
    // Определяем сектора
    if (index < MAX_RECIPES_PER_SECTOR) {
        source_sector = RECIPES_SECTOR_2;
        target_sector = RECIPES_SECTOR_3;
    } else {
        source_sector = RECIPES_SECTOR_3;
        target_sector = RECIPES_SECTOR_2;
    }
    
    // Стираем целевой сектор
    W25qxx_EraseSector(target_sector);
    
    // Копируем все рецепты из исходного сектора в целевой
    for (uint8_t i = 0; i < MAX_RECIPES_PER_SECTOR; i++) {
        uint32_t source_page = GetRecipePageAddress(source_sector, i);
        uint32_t target_page = GetRecipePageAddress(target_sector, i);
        
        Recipe_t recipe;
        
        // Читаем рецепт из исходного сектора
        W25qxx_ReadPage((uint8_t*)&recipe, source_page, 0, sizeof(Recipe_t));
        
        // Если страница не пустая и это наш индекс - обновляем рецепт
        uint8_t global_index = (source_sector == RECIPES_SECTOR_2) ? i : (i + MAX_RECIPES_PER_SECTOR);
        if (global_index == index) {
            recipe = *new_recipe;
            recipe.crc16 = CRC16_Calculate((uint8_t*)&recipe, sizeof(Recipe_t) - 2);
        }
        
        // Записываем в целевой сектор (только непустые рецепты)
        if (!W25qxx_IsEmptyPage(source_page, 0, sizeof(Recipe_t))) {
            W25qxx_WritePage((uint8_t*)&recipe, target_page, 0, sizeof(Recipe_t));
        }
    }
    
    // Обновляем активный сектор
    active_sector = target_sector;
    
    // Обновляем счетчик рецептов
    if (index >= recipe_count) {
        recipe_count = index + 1;
    }
    
    return 1;
}

/*
Чтение рецепта из Flash
*/
uint8_t Recipe_ReadFromFlash(Recipe_t* recipe, uint8_t index) {
    if (index >= recipe_count) return 0;
    
    uint8_t target_sector, page_index;
    
    if (index < MAX_RECIPES_PER_SECTOR) {
        target_sector = RECIPES_SECTOR_2;
        page_index = index;
    } else {
        target_sector = RECIPES_SECTOR_3;
        page_index = index - MAX_RECIPES_PER_SECTOR;
    }
    
    uint32_t page_address = GetRecipePageAddress(target_sector, page_index);
    
    // Читаем рецепт
    W25qxx_ReadPage((uint8_t*)recipe, page_address, 0, sizeof(Recipe_t));
    
    // Проверяем CRC
    uint16_t calculated_crc = CRC16_Calculate((uint8_t*)recipe, sizeof(Recipe_t) - 2);
    if (calculated_crc != recipe->crc16) {
        return 0;  // Ошибка CRC
    }
    
    return 1;
}

/*
Получение количества сохраненных рецептов
*/
//uint8_t Recipe_GetCount(void) {
//    return recipe_count;
//}

/* 
Инициализация системы хранения рецептов
*/
void Recipe_StorageInit(void) {
    // Определяем активный сектор (где больше записанных рецептов)
    uint8_t count_sector2 = 0;
    uint8_t count_sector3 = 0;
    
    // Считаем рецепты в секторе 2
    for (uint8_t i = 0; i < MAX_RECIPES_PER_SECTOR; i++) {
        uint32_t page_addr = GetRecipePageAddress(RECIPES_SECTOR_2, i);
        if (!W25qxx_IsEmptyPage(page_addr, 0, sizeof(Recipe_t))) {
            count_sector2++;
        }
    }
    
    // Считаем рецепты в секторе 3
    for (uint8_t i = 0; i < MAX_RECIPES_PER_SECTOR; i++) {
        uint32_t page_addr = GetRecipePageAddress(RECIPES_SECTOR_3, i);
        if (!W25qxx_IsEmptyPage(page_addr, 0, sizeof(Recipe_t))) {
            count_sector3++;
        }
    }
    
    // Активный сектор - тот, где больше рецептов
    // Исправляем подсчет общего количества
    if (count_sector2 >= count_sector3) {
        active_sector = RECIPES_SECTOR_2;
        recipe_count = count_sector2;
    } else {
        active_sector = RECIPES_SECTOR_3;
        recipe_count = count_sector3;  // Исправлено: убираем + MAX_RECIPES_PER_SECTOR
    }
    
    // Общее количество рецептов - сумма из обоих секторов
    total_recipes = count_sector2 + count_sector3;
}

/*
Удаление рецепта
*/
uint8_t Recipe_DeleteFromFlash(uint8_t index) {
    if (index >= recipe_count) return 0;
    
    // Для удаления используем wear leveling, но с пустым рецептом
    Recipe_t empty_recipe = {0};
    empty_recipe.recipe_uid = 0xFFFFFFFF;  // Помечаем как удаленный
    
    return Recipe_SaveWithWearLeveling(&empty_recipe, index);
}



/*
Загрузка рецепта из FLASH памяти
*/
uint8_t Recipe_LoadFromFlash(uint8_t recipe_index) {
    if (recipe_index >= recipe_count) return 0;
    
    uint8_t target_sector, page_index;
    
    // Определяем сектор и индекс страницы
    if (recipe_index < MAX_RECIPES_PER_SECTOR) {
        target_sector = RECIPES_SECTOR_2;
        page_index = recipe_index;
    } else {
        target_sector = RECIPES_SECTOR_3;
        page_index = recipe_index - MAX_RECIPES_PER_SECTOR;
    }
    
    uint32_t page_address = GetRecipePageAddress(target_sector, page_index);
    
    // Читаем рецепт из FLASH
    W25qxx_ReadPage((uint8_t*)&current_recipe, page_address, 0, sizeof(Recipe_t));
    
    // Проверяем CRC
    uint16_t calculated_crc = CRC16_Calculate((uint8_t*)&current_recipe, sizeof(Recipe_t) - 2);
    if (calculated_crc != current_recipe.crc16) {
        return 0; // Ошибка CRC
    }
    
    // Проверяем, не удален ли рецепт
    if (current_recipe.recipe_uid == 0xFFFFFFFF) {
        return 0; // Рецепт удален
    }
    
    current_recipe_index = recipe_index;
    return 1;
}

/*
Получение списка рецептов
*/
uint8_t Recipe_GetList(char recipe_names[][32], uint8_t max_count) {
    uint8_t count = (recipe_count < max_count) ? recipe_count : max_count;
    uint8_t found_count = 0;
    
    for (uint8_t i = 0; i < count && found_count < max_count; i++) {
        Recipe_t temp_recipe;
        
        // Используем функцию загрузки для проверки CRC и валидности
        if (Recipe_LoadFromFlash(i)) {
            // Копируем имя, если рецепт валиден
            memcpy(recipe_names[found_count], current_recipe.name, current_recipe.name_length);
            recipe_names[found_count][current_recipe.name_length] = '\0';
            found_count++;
        }
    }
    
    return found_count;
}

/*
Альтернативная версия GetList только для чтения имен (быстрее, но без проверки CRC)
*/
uint8_t Recipe_GetListFast(char recipe_names[][32], uint8_t max_count) {
    uint8_t found_count = 0;
    
    // Проверяем оба сектора
    for (uint8_t sector = RECIPES_SECTOR_2; sector <= RECIPES_SECTOR_3 && found_count < max_count; sector++) {
        uint8_t max_pages = (sector == RECIPES_SECTOR_2) ? MAX_RECIPES_PER_SECTOR : 
                           (max_count - found_count);
        
        for (uint8_t page_index = 0; page_index < max_pages && found_count < max_count; page_index++) {
            uint32_t page_address = GetRecipePageAddress(sector, page_index);
            Recipe_t temp_recipe;
            
            // Читаем только начало структуры с именем
            W25qxx_ReadPage((uint8_t*)&temp_recipe, page_address, 0, 
                           offsetof(Recipe_t, name) + 32); // Читаем до конца имени
            
            // Проверяем, что рецепт существует (не пустая страница и не удален)
            if (temp_recipe.recipe_uid != 0xFFFFFFFF && temp_recipe.recipe_uid != 0) {
                // Копируем имя
                uint8_t name_len = temp_recipe.name_length;
                if (name_len > 31) name_len = 31;
                
                memcpy(recipe_names[found_count], temp_recipe.name, name_len);
                recipe_names[found_count][name_len] = '\0';
                found_count++;
            }
        }
    }
    
    return found_count;
}

/*
Получение информации о рецепте без загрузки полной структуры
*/
uint8_t Recipe_GetInfo(uint8_t recipe_index, char* name, uint16_t* original_gravity) {
    if (recipe_index >= recipe_count) return 0;
    
    uint8_t target_sector, page_index;
    
    if (recipe_index < MAX_RECIPES_PER_SECTOR) {
        target_sector = RECIPES_SECTOR_2;
        page_index = recipe_index;
    } else {
        target_sector = RECIPES_SECTOR_3;
        page_index = recipe_index - MAX_RECIPES_PER_SECTOR;
    }
    
    uint32_t page_address = GetRecipePageAddress(target_sector, page_index);
    Recipe_t temp_recipe;
    
    // Читаем минимально необходимые поля
    W25qxx_ReadPage((uint8_t*)&temp_recipe, page_address, 0, 
                   offsetof(Recipe_t, original_gravity) + 2);
    
    // Проверяем валидность
    if (temp_recipe.recipe_uid == 0xFFFFFFFF) return 0;
    
    if (name) {
        uint8_t name_len = temp_recipe.name_length;
        if (name_len > 31) name_len = 31;
        memcpy(name, temp_recipe.name, name_len);
        name[name_len] = '\0';
    }
    
    if (original_gravity) {
        *original_gravity = temp_recipe.original_gravity;
    }
    
    return 1;
}

/*
Получение количества сохраненных рецептов
*/
uint8_t Recipe_GetCount(void) {
    return total_recipes;  // Используем глобальную переменную, которая обновляется при инициализации
}

/*
Получение списка названий рецептов
*/
uint8_t Recipe_GetNames(char names[][32], uint8_t max_names) {
    uint8_t found_count = 0;
    
    // Проверяем оба сектора
    for (uint8_t sector = RECIPES_SECTOR_2; sector <= RECIPES_SECTOR_3 && found_count < max_names; sector++) {
        uint8_t pages_in_sector = (sector == RECIPES_SECTOR_2) ? MAX_RECIPES_PER_SECTOR : MAX_RECIPES_PER_SECTOR;
        
        for (uint8_t page_index = 0; page_index < pages_in_sector && found_count < max_names; page_index++) {
            uint32_t page_address = GetRecipePageAddress(sector, page_index);
            
            // Проверяем, не пустая ли страница
            if (!W25qxx_IsEmptyPage(page_address, 0, sizeof(Recipe_t))) {
                Recipe_t temp_recipe;
                
                // Читаем только заголовок рецепта с именем
                W25qxx_ReadPage((uint8_t*)&temp_recipe, page_address, 0, 
                               offsetof(Recipe_t, name) + 32);
                
                // Проверяем, что рецепт не удален
                if (temp_recipe.recipe_uid != 0xFFFFFFFF && temp_recipe.recipe_uid != 0) {
                    // Копируем имя
                    uint8_t name_len = temp_recipe.name_length;
                    if (name_len > 31) name_len = 31;
                    
                    memcpy(names[found_count], temp_recipe.name, name_len);
                    names[found_count][name_len] = '\0';  // Добавляем терминатор
                    found_count++;
                }
            }
        }
    }
    
    return found_count;
}

/**/
uint8_t receptBuf[] = {
0x12, 0x34, 0x56, 0x78, 0x01, 0x00, 0x0B, 0x41, 
0x6D, 0x65, 0x72, 0x69, 0x63, 0x61, 0x6E, 0x20, 
0x49, 0x50, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x45, 0x10, 0x0A, 0x10, 0x3C, 
0x00, 0x92, 0x02, 0x08, 0x03, 0xA4, 0x0C, 0x3C, 
0x00, 0x00, 0x88, 0x0D, 0x1E, 0x00, 0x01, 0x84, 
0x0E, 0x0A, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 
0x02, 0x3C, 0x00, 0x19, 0x00, 0x00, 0x0A, 0x00, 
0x0F, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xDE, 0x0C, 0x0E, 0x00, 0x5A, 
0x79
};
/**/
void preSetRecepteToFlash(void){
	W25qxx_EraseSector(RECIPES_SECTOR_2);
	Recipe_SaveToFlash((Recipe_t*)receptBuf, total_recipes);
	for(uint8_t i = 0; i < 10; i++){
		receptBuf[i] = 0;
	}
	Recipe_LoadFromFlash(0); 
}



