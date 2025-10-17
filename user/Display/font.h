#ifndef _FONT_H
#define _FONT_H
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup



typedef struct _font_char_info
{
  uint8_t CharWidth;
 // uint8_t CharHeight;
  uint16_t Offset;
}FONT_CHAR_INFO;

//typedef struct _test_font_char_info
//{
//  uint16_t CharWidth;
//  uint16_t CharHeight;
//  uint16_t Offset;
//}TEST_FONT_CHAR_INFO;

////////////////////////////////////////////////////////////////////////////////
typedef struct _font_info
{
  uint8_t CharacterHeight;//Высота символа
  uint16_t StartCharacter;//Начальный символ в массиве
  uint16_t EndCharacter;//Конечный символ в сассиве
  uint8_t spase;//Длина пробела между символами
  FONT_CHAR_INFO *Descriptors;//разместить ао FLASH?? Указатель на описание параметров буквы
  uint8_t *Bitmaps;//разместить ао FLASH?? Указатель собственно на массив с буквой
}FONT_INFO;


#endif
