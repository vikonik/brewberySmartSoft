/*
массив с рецептом
0x12 0x34 0x56 0x78 0x01 0x00 0x0B 0x41 0x6D 0x65 0x72 0x69 0x63 0x61 0x6E 0x20 0x49 0x50 0x41 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x45 0x10 0x0A 0x10 0x3C 0x00 0x92 0x02 0x08 0x03 0xA4 0x0C 0x3C 0x00 0x00 0x88 0x0D 0x1E 0x00 0x01 0x84 0x0E 0x0A 0x00 0x02 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x3C 0x00 0x02 0x3C 0x00 0x19 0x00 0x00 0x0A 0x00 0x0F 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xDE 0x0C 0x0E 0x00 0x5A 0x79


*/
#include "recept.h"

const char mashTypeProtein[] = "Белковая пауза";
const char mashTypeSaccherification[] = "Осахаривание";
const char mashTypeMasOut[] = "Мэш-аут";

/*
Параметр									Темное (Dunkel)							Светлое (Pilsner)									Пшеничное (Hefeweizen)
Температура затирания						68°C (одна пауза)			52°C → 63°C → 72°C (три паузы)		45°C → 67°C (две паузы)
Время затирания									70 мин								90 мин														75 мин
Время кипячения									60 мин								90 мин														60 мин
Горечь (IBU)										18 (низкая)						35 (умеренная)										12 (очень низкая)
Температура ферментации					10°C (холодная)				11°C (холодная)										21°C (теплая)
Время ферментации								21 день								28 дней														14 дней
Особенности							Простая схема, 								Сложная схема, 										Минимальное охмеление, 
												акцент на темных солодах			благородные хмели									специальные дрожжи
*/
/* ====== ТЕМНОЕ ПИВО (Dunkel / Munich Dunkel) ======
 * Характеристики: Мальтийный, хлебный вкус, низкая горечь
 * Особенности: Преобладание темных солодов, однократная настойка
 */
const Recipe_t pshenichnoe_beer = {
    .recipe_uid = 0x444E4B4C,      // 'DNKL' в ASCII
    .version = 1,
    .name_length = 13,
    .name = "Пшеничное",

    .original_gravity = 1052,      // OG = 1.052
    .target_gravity = 1014,        // FG = 1.014
    .ibu = 18,                     // Низкая горечь
    .abv = 500,                    // 5.0% ABV
    .beer_color = 20,              // Темный цвет (20 SRM)

    // Затирание: простая однократная настойка
    .mash_stages_count = 2,
    .mash_stages = {
        { .temperature = 680, .time = 70, .stage_type = STAGE_TYPE_SACCHARIFICATION }, // 68°C - для полнотелого тела
        { .temperature = 780, .time = 10, .stage_type = STAGE_TYPE_MASH_OUT }
    },

    // Кипячение: минимальное охмеление
    .boil_time = 60,
    .hop_additions_count = 2,
    .hops = {
        { .time = 60, .amount = 250, .hop_type = HOP_TYPE_HALLERTAUER }, // 25г на горечь
        { .time = 15, .amount = 50,  .hop_type = HOP_TYPE_TETTNANG }     // 5г на аромат
    },

    // Ферментация: лагерные дрожжи, холодное брожение
    .fermentation_temp = 100,      // 10.0°C - низкая температура для лагерных дрожжей
    .fermentation_days = 21,       // Длительная ферментация

    .crc16 = 0x0000 // Рассчитывается реальной функцией
};

/* ====== СВЕТЛОЕ ПИВО (Pilsner / Helles) ======
 * Характеристики: Чистый солодовый профиль, яркая хмелевая горчинка
 * Особенности: Многоступенчатое затирание, благородные хмели
 */
const Recipe_t darck_beer = {
    .recipe_uid = 0x50494C53,      // 'PILS' в ASCII
    .version = 1,
    .name_length = 12,
    .name = "Темное",

    .original_gravity = 1048,      // OG = 1.048
    .target_gravity = 1010,        // FG = 1.010 - сухое пиво
    .ibu = 35,                     // Умеренная горечь
    .abv = 500,                    // 5.0% ABV
    .beer_color = 4,               // Светло-золотой (4 SRM)

    // Затирание: многоступенчатое для высокой сбраживаемости
    .mash_stages_count = 4,
    .mash_stages = {
        { .temperature = 520, .time = 20, .stage_type = STAGE_TYPE_PROTEIN },        // 52°C - белковая пауза
        { .temperature = 630, .time = 30, .stage_type = STAGE_TYPE_SACCHARIFICATION }, // 63°C - мальтозная пауза
        { .temperature = 720, .time = 40, .stage_type = STAGE_TYPE_SACCHARIFICATION }, // 72°C - основная осахаривающая
        { .temperature = 780, .time = 10, .stage_type = STAGE_TYPE_MASH_OUT }
    },

    // Кипячение: выраженное охмеление благородными хмелями
    .boil_time = 90,               // Длительное кипячение для уменьшения DMS
    .hop_additions_count = 3,
    .hops = {
        { .time = 90, .amount = 200, .hop_type = HOP_TYPE_HALLERTAUER }, // 20г - горечь
        { .time = 30, .amount = 100, .hop_type = HOP_TYPE_TETTNANG },    // 10г - вкус
        { .time = 5,  .amount = 80,  .hop_type = HOP_TYPE_SAAZ }         // 8г - аромат
    },

    // Ферментация: лагерные дрожжи с длительным холодным дображиванием
    .fermentation_temp = 110,      // 11.0°C
    .fermentation_days = 28,       // Длительная ферментация + лагеринг

    .crc16 = 0x0000
};

/* ====== ПШЕНИЧНОЕ ПИВО (Hefeweizen) ======
 * Характеристики: Фруктово-пряные тона, мутность, пышная пена
 * Особенности: Высокое содержание пшеничного солода, специальные дрожжи
 */
const Recipe_t light_beer = {
    .recipe_uid = 0x4846575A,      // 'HFWZ' в ASCII
    .version = 1,
    .name_length = 14,
    .name = "Светоле",

    .original_gravity = 1050,      // OG = 1.050
    .target_gravity = 1012,        // FG = 1.012
    .ibu = 12,                     // Очень низкая горечь
    .abv = 500,                    // 5.0% ABV
    .beer_color = 6,               // Соломенный цвет (6 SRM)

    // Затирание: простая схема, акцент на пшеничном солоде
    .mash_stages_count = 3,
    .mash_stages = {
        { .temperature = 450, .time = 15, .stage_type = STAGE_TYPE_ACID_REST },     // 45°C - кислотная пауза (опционально)
        { .temperature = 670, .time = 60, .stage_type = STAGE_TYPE_SACCHARIFICATION }, // 67°C
        { .temperature = 780, .time = 10, .stage_type = STAGE_TYPE_MASH_OUT }
    },

    // Кипячение: минимальное охмеление чтобы не мешать дрожжевому профилю
    .boil_time = 60,
    .hop_additions_count = 1,
    .hops = {
        { .time = 60, .amount = 150, .hop_type = HOP_TYPE_HALLERTAUER } // 15г - только на горечь
    },

    // Ферментация: специальные дрожжи для пшеничного пива, повышенная температура
    .fermentation_temp = 210,      // 21.0°C - для выраженного эфирного профиля
    .fermentation_days = 14,

    .crc16 = 0x0000
};

/**/


