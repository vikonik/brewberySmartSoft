#ifndef RECEPT_H
#define RECEPT_H


#include <stdint.h> // Для использования целочисленных типов фиксированной длины
extern const char mashTypeProtein[];
extern const char mashTypeSaccherification[];
extern const char mashTypeMasOut[];

#pragma pack(push, 1)
typedef enum {
	MashType_PROTEIN,//Белковая
	MashType_SACCHARIFICATION,//Осахаривание
	MashType_MASH_OUT
}MashType_t;

// Структура для одной температурной паузы затирания
typedef struct {
    uint16_t 		temperature;  // Температура (умноженная на 10 для избежания float) -> 655 = 65.5°C
    uint16_t 		time;         // Время выдержки в минутах
    MashType_t  stage_type;   // Тип паузы: 0 - Белковая, 1 - Осахаривание, 2 - Мэш-аут, etc.
}MashStage;

// Структура для добавления хмеля
typedef struct {
    uint16_t time;         // Время добавления (мин от начала кипячения)
    uint16_t amount;       // Количество (грамм, умноженное на 10 -> 105 = 10.5г)
    uint8_t  hop_type;     // Тип хмеля (индекс в заранее определенном массиве названий)
}HopAddition;

// Основная структура рецепта
typedef struct {
    // Заголовок рецепта
    uint32_t recipe_uid;           // Уникальный ID рецепта
    uint16_t version;              // Версия структуры данных
    uint8_t  name_length;          // Длина названия рецепта
    char     name[32];             // Название рецепта ("American IPA")

    // Параметры пива
    uint16_t original_gravity;     // Начальная плотность (OG * 1000 -> 1045 = 1.045)
    uint16_t target_gravity;       // Конечная плотность (FG * 1000 -> 1010 = 1.010)
    uint16_t ibu;                  // Горечь (IBU)
    uint16_t abv;                  // Крепость (ABV * 100 -> 650 = 6.5%)
    uint8_t  beer_color;           // Цвет по SRM (стандарт Reference Method)

    // Процесс затирания
    uint8_t  mash_stages_count;    // Количество пауз затирания (макс. 5)
    MashStage mash_stages[5];      // Массив пауз затирания

    // Процесс кипячения
    uint16_t boil_time;            // Общее время кипячения в минутах
    uint8_t  hop_additions_count;  // Количество добавок хмеля (макс. 8)
    HopAddition hops[8];           // Массив добавок хмеля

    // Процесс ферментации
    uint16_t fermentation_temp;    // Температура ферментации (умноженная на 10 -> 190 = 19.0°C)
    uint16_t fermentation_days;    // Длительность первичной ферментации (дней)

    // Служебная информация
    uint16_t crc16;                // Контрольная сумма структуры для проверки целостности
} Recipe_t;
#pragma pack(pop)

// Пример предопределенных типов (для экономии памяти)
#define HOP_TYPE_CITRA     0
#define HOP_TYPE_AMARILLO  1
#define HOP_TYPE_SIMCOE    2
#define HOP_TYPE_CASCADE   3
// Дополнительные типы хмелей для этих рецептов
#define HOP_TYPE_HALLERTAUER  4
#define HOP_TYPE_TETTNANG     5
#define HOP_TYPE_SAAZ         6

// Дополнительные типы пауз
#define STAGE_TYPE_ACID_REST  3    // Кислотная пауза для пшеничного пива
// ... и так далее

#define STAGE_TYPE_PROTEIN 0
#define STAGE_TYPE_SACCHARIFICATION 1
#define STAGE_TYPE_MASH_OUT 2


#endif
