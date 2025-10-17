// Fast ST7567 128x64 LCD graphics library (with frambuffer)
// (C) 2020 by Pawel A. Hernik

/*
 128x64 ST7567 connections in SPI mode (only 5-6 wires between LCD and MCU):

 #01 LED  -> D6, GND or any pin via resistor
 #02 RST  -> D9 or any pin
 #03 CS   -> D10 or any pin
 #04 DC   -> D8 or any pin
 #05 SCK  -> D13/SCK
 #06 SDI  -> D11/MOSI
 #07 3V3  -> VCC (3.3V)
 #08 GND  -> GND
*/

#ifndef _ST7567_FB_H
#define _ST7567_FB_H

#ifdef USE_STM
#include "stm32f10x.h"                  // Device header

#else
#include "MDR32FxQI_config.h"           // Milandr::Device:Startup


#endif

#include <stdbool.h> 
#include "font.h"
// ------------
// remove define for software SPI
#define USE_HW_SPI
// ------------

//#include <Arduino.h>
//#include <avr/pgmspace.h>

#define SCR_WD  128
#define SCR_HT  64
#define SCR_HT8 8  // SCR_HT/8

//#define ALIGN_LEFT    0
//#define ALIGN_RIGHT  -1 
//#define ALIGN_CENTER -2

#define ST7567_SET 1
#define ST7567_CLR 0
#define ST7567_XOR 2

typedef enum{
    NORMAL,
    INVERSE,
    SWITCH
}Inverse_t;

typedef enum{
    ALIGN_LEFT,
    ALIGN_CENTR,
    ALIGN_RIGHT
}Align_t;


typedef struct _propFont
{
  const uint8_t* font;
  int8_t xSize;
  uint8_t ySize;
  uint8_t firstCh;
  uint8_t lastCh;
  uint8_t minCharWd;
  uint8_t minDigitWd;
}_propFont_t;

// ---------------------------------
// extern _propFont_t  cfont;
 extern FONT_INFO  cfont;
 extern uint8_t cr;  // carriage return mode for printStr
 extern uint8_t dualChar;
 extern uint8_t invertCh;
 extern uint8_t spacing;	
	
//-----------------------------------------

  uint8_t sendSPI(uint8_t v); // costs about 350B of flash
  void sendCmd(uint8_t cmd);
  void sendData(uint8_t data);
  void ST7567_FB_init(int contrast);
  void ST7567_FB_begin(void);
  void ST7567_FB_initCmds(void);
  void ST7567_FB_display(void);
  void ST7567_FB_copy(uint8_t x, uint8_t y8, uint8_t wd, uint8_t ht8);
  void ST7567_FB_gotoXY(uint8_t x, uint8_t y);
  void ST7567_FB_sleep(bool mode);
  void ST7567_FB_setContrast(uint8_t val);
  void ST7567_FB_setScroll(uint8_t val);
  void ST7567_FB_displayInvert(bool mode);
  void ST7567_FB_displayOn(bool mode);
  void ST7567_FB_displayMode(uint8_t val);
  void ST7567_FB_setRotation(int mode);

  void ST7567_FB_cls(void);
 // void ST7567_FB_clearDisplay(void) { ST7567_FB_cls(); }
  void ST7567_FB_drawPixel(uint8_t x, uint8_t y, uint8_t col);
  void ST7567_FB_drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col);
  void ST7567_FB_drawLineH(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col);
  void ST7567_FB_drawLineV(uint8_t x,  uint8_t y0, uint8_t y1, uint8_t col);
  void ST7567_FB_drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col);
  void ST7567_FB_drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col);
  void ST7567_FB_drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col);
  void ST7567_FB_drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col);
  void ST7567_FB_drawRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void ST7567_FB_drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col);
  void ST7567_FB_fillRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void ST7567_FB_fillRectD(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t col);
  void ST7567_FB_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col);
  void ST7567_FB_fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col);
  void ST7567_FB_fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col);
  void ST7567_FB_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void ST7567_FB_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void ST7567_FB_fillTriangleD(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
  void ST7567_FB_setDither(int8_t s);
  int _ST7567_FB_drawBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h, Inverse_t invers);
  int _ST7567_FB_clerBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h);
  int ST7567_FB_drawBitmap(const uint8_t *bmp, int x, uint8_t y, Inverse_t invers);
  int ST7567_FB_clerBitmap(const uint8_t *bmp, int x, uint8_t y);
  int ST7567_FB_invertBitmap(const uint8_t *bmp, int x, uint8_t y);
  
 // void ST7567_FB_setFont(const uint8_t* f);
  void ST7567_FB_setFont(const FONT_INFO* f);
  void ST7567_FB_printText(uint16_t x, uint16_t y, char *str, Inverse_t invers);
  void ST7567_FB_setCR(uint8_t _cr);
  void ST7567_FB_setInvert(uint8_t _inv);
  void ST7567_FB_setFontMinWd(uint8_t wd);
  void ST7567_FB_setCharMinWd(uint8_t wd);
  void ST7567_FB_setDigitMinWd(uint8_t wd);
  int ST7567_FB_printChar(int xpos, int ypos, unsigned char c);
  int ST7567_FB_printStr(int xpos, int ypos, char *str);
  int ST7567_FB_charWidth(FONT_CHAR_INFO *CharInfo, uint8_t *c);
  int ST7567_FB_fontHeight(void);
  int ST7567_FB_strWidth(char *txt);
  unsigned char ST7567_FB_convertPolish(unsigned char _c);
  bool ST7567_FB_isNumber(uint8_t ch);
  bool ST7567_FB_isNumberExt(uint8_t ch);
  void ST7567_FB_setIsNumberFun(bool (*fun)(uint8_t));
  uint8_t ST7567_FB_align_text(char* buf, Align_t align);

void ST7567_FB_test(void);

#endif

