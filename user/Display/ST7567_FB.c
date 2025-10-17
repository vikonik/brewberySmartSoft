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
#include "font.h"
#include "ST7567_FB.h"
#include "allDefenition.h"
#include "string.h"
#include "stdlib.h"
#include <ctype.h>

#ifdef USE_STM
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_spi.h"              // Keil::Device:StdPeriph Drivers:SPI

#define CS_IDLE     GPIO_SetBits(LCD_CS_PORT, LCD_CS_PIN)
#define CS_ACTIVE   GPIO_ResetBits(LCD_CS_PORT, LCD_CS_PIN)
#define DC_DATA     GPIO_SetBits(LCD_D_C_PORT, LCD_D_C_PIN)
#define DC_COMMAND  GPIO_ResetBits(LCD_D_C_PORT, LCD_D_C_PIN)
#else
#include "MDR32FxQI_port.h"             // Milandr::Drivers:PORT
#include "MDR32FxQI_ssp.h"              // Milandr::Drivers:SSP


//Здесь меняем под свой1 контроллер
#define CS_IDLE     PORT_SetBits(LCD_CS_PORT, LCD_CS_PIN)
#define CS_ACTIVE   PORT_ResetBits(LCD_CS_PORT, LCD_CS_PIN)
#define DC_DATA     PORT_SetBits(LCD_D_C_PORT, LCD_D_C_PIN)
#define DC_COMMAND  PORT_ResetBits(LCD_D_C_PORT, LCD_D_C_PIN)
#endif


#define fontuint8_t(x) *(&cfont.font[x])





#define MAX(a,b)					((a) > (b) ? (a) : (b))
//------------------------------
// ST7567 Commands
#define ST7567_POWER_ON         0x2F // internal power supply on
#define ST7567_POWER_CTL        0x28 // internal power supply off
#define ST7567_CONTRAST         0x80 // 0x80 + (0..31)
#define ST7567_SEG_NORMAL       0xA0 // SEG remap normal
#define ST7567_SEG_REMAP        0xA1 // SEG remap reverse (flip horizontal)
#define ST7567_DISPLAY_NORMAL   0xA4 // display ram content
#define ST7567_DISPLAY_TEST     0xA5 // all pixels on
#define ST7567_INVERT_OFF       0xA6 // not inverted
#define ST7567_INVERT_ON        0xA7 // inverted
#define ST7567_DISPLAY_ON       0XAF // display on
#define ST7567_DISPLAY_OFF      0XAE // display off
#define ST7567_STATIC_OFF       0xAC
#define ST7567_STATIC_ON        0xAD
#define ST7567_SCAN_START_LINE  0x40 // scrolling 0x40 + (0..63)
#define ST7567_COM_NORMAL       0xC0 // COM remap normal
#define ST7567_COM_REMAP        0xC8 // COM remap reverse (flip vertical)
#define ST7567_SW_RESET         0xE2 // connect RST pin to GND and rely on software reset
#define ST7567_NOP              0xE3 // no operation
#define ST7567_TEST             0xF0

#define ST7567_COL_ADDR_H       0x10 // x pos (0..95) 4 MSB
#define ST7567_COL_ADDR_L       0x00 // x pos (0..95) 4 LSB
#define ST7567_PAGE_ADDR        0xB0 // y pos, 8.5 rows (0..8)
#define ST7567_RMW              0xE0
#define ST7567_RMW_CLEAR        0xEE

#define ST7567_BIAS_9           0xA2 
#define ST7567_BIAS_7           0xA3

#define ST7567_VOLUME_FIRST     0x81
#define ST7567_VOLUME_SECOND    0x00

#define ST7567_RESISTOR_RATIO   0x20
#define ST7567_STATIC_REG       0x0
#define ST7567_BOOSTER_FIRST    0xF8
#define ST7567_BOOSTER_234      0
#define ST7567_BOOSTER_5        1
#define ST7567_BOOSTER_6        3
//------------------------------

const uint8_t initData[] = {
  ST7567_BIAS_7,
  ST7567_SEG_NORMAL,
  ST7567_COM_REMAP,
  ST7567_POWER_CTL | 0x4,
  ST7567_POWER_CTL | 0x6, // turn on voltage regulator (VC=1, VR=1, VF=0)
  ST7567_POWER_CTL | 0x7, // turn on voltage follower (VC=1, VR=1, VF=1)
  ST7567_RESISTOR_RATIO | 0x6, // set lcd operating voltage (regulator resistor, ref voltage resistor)
  ST7567_SCAN_START_LINE+0,
  //ST7567_DISPLAY_OFF,
  ST7567_DISPLAY_NORMAL
};
// ----------------------------------------------------------------

  static uint8_t scr[SCR_WD*SCR_HT8];
  uint8_t scrWd = SCR_WD;
  uint8_t scrHt = SCR_HT8;
  uint8_t dcPin, csPin, rstPin;
  uint8_t sdiPin, clkPin;
  int8_t rotation;

  static uint8_t ystab[8];
  static uint8_t yetab[8];
  static uint8_t pattern[4];
  static uint8_t ditherTab[4*17];

//private:
  
 // _propFont_t  cfont;
  FONT_INFO cfont;//вместо _propFont_t
  uint8_t cr;  // carriage return mode for printStr
  uint8_t dualChar;
  uint8_t invertCh;
  uint8_t spacing = 1;	
	
	
	bool (*isNumberFun)(uint8_t ch);
	//-----------------------------------------------------------------------
uint8_t  sendSPI(uint8_t v)
{
    	/* loop while data register in not emplty */
	while (RESET == SSP_GetFlagStatus(MDR_SSP1,SSP_FLAG_TFE)  );
	/* send byte through the SPI0 peripheral */
	SSP_SendData(MDR_SSP1, v);   
    
	/* wait to receive a byte */
	while(RESET == SSP_GetFlagStatus(MDR_SSP1,SSP_FLAG_RNE));
	/* return the byte read from the SPI bus */
	return SSP_ReceiveData(MDR_SSP1);

}
// ----------------------------------------------------------------
 void sendCmd(uint8_t cmd)
{
  DC_COMMAND;
  sendSPI(cmd);
}
// ----------------------------------------------------------------
void sendData(uint8_t data)
{
  DC_DATA;
  sendSPI(data);
}
// ----------------------------------------------------------------

// ----------------------------------------------------------------
uint8_t ST7567_FB_scr[SCR_WD*SCR_HT8];

void ST7567_FB_init(int contrast)
{

	
  ST7567_FB_initCmds();
  ST7567_FB_setContrast(contrast);
  //setRotation(0);
}
// ----------------------------------------------------------------
void ST7567_FB_initCmds(void)
{
  CS_ACTIVE;
  for(int i=0; i<sizeof(initData); i++) 
		sendCmd(*(initData+i));
  /*
  sendCmd(ST7567_BIAS_7);
  sendCmd(ST7567_SEG_NORMAL);
  sendCmd(ST7567_COM_REMAP);

  sendCmd(ST7567_POWER_CTL | 0x4); delay(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  sendCmd(ST7567_POWER_CTL | 0x6); delay(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  sendCmd(ST7567_POWER_CTL | 0x7); delay(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  sendCmd(ST7567_RESISTOR_RATIO | 0x6);

  // Initial display line
  sendCmd(ST7567_SCAN_START_LINE+0); 
  sendCmd(ST7567_DISPLAY_ON);
  sendCmd(ST7567_DISPLAY_NORMAL);
  */
  CS_IDLE;
}
//-----------------------------------------------------------------
  void ST7567_FB_begin(void) { ST7567_FB_init(7); }
// ----------------------------------------------------------------
void ST7567_FB_gotoXY(uint8_t x, uint8_t y)
{
  CS_ACTIVE;
  sendCmd(ST7567_PAGE_ADDR | y);
  sendCmd(ST7567_COL_ADDR_H | (x >> 4));
  sendCmd(ST7567_COL_ADDR_L | (x & 0xf));
  sendCmd(ST7567_RMW);
  //CS_IDLE;
}
// ----------------------------------------------------------------
void ST7567_FB_sleep(bool mode)
{
  if(mode) {
    ST7567_FB_cls();
    ST7567_FB_display();
    CS_ACTIVE;
    sendCmd(ST7567_DISPLAY_OFF); // power down
    sendCmd(ST7567_DISPLAY_TEST); // all pixels on (saves power)
    CS_IDLE;
  } else ST7567_FB_initCmds();
}
// ----------------------------------------------------------------
// 0..31
void ST7567_FB_setContrast(uint8_t val)
{
  CS_ACTIVE;
  sendCmd(ST7567_VOLUME_FIRST);
  sendCmd(ST7567_VOLUME_SECOND | (val & 0x3f));
  CS_IDLE;
}
// ----------------------------------------------------------------
// 0..63
void ST7567_FB_setScroll(uint8_t val)
{
  CS_ACTIVE;
  sendCmd(ST7567_SCAN_START_LINE|(val&0x3f));
  CS_IDLE;
}
// ----------------------------------------------------------------
// 0..3, only 0 and 2 supported on ST7567
void ST7567_FB_setRotation(int mode)
{
  rotation = mode;
  CS_ACTIVE;
  switch(mode) {
    case 0:
      sendCmd(ST7567_SEG_NORMAL);
      sendCmd(ST7567_COM_REMAP);
      break;
    case 2:
      sendCmd(ST7567_SEG_REMAP);
      sendCmd(ST7567_COM_NORMAL);
      break;
  }
  CS_IDLE;
}
// ----------------------------------------------------------------
void ST7567_FB_displayInvert(bool mode)
{
  CS_ACTIVE;
  sendCmd(mode ? ST7567_INVERT_ON : ST7567_INVERT_OFF);
  CS_IDLE;
}
// ----------------------------------------------------------------
void ST7567_FB_displayOn(bool mode)
{
  CS_ACTIVE;
  sendCmd(mode ? ST7567_DISPLAY_ON : ST7567_DISPLAY_OFF);
  CS_IDLE;
}
// ----------------------------------------------------------------
// val=ST7567_POWER_ON, ST7567_POWER_OFF, ST7567_DISPLAY_NORMAL, ST7567_DISPLAY_TEST
//     ST7567_INVERT_OFF, ST7567_INVERT_ON, ST7567_DISPLAY_ON, ST7567_DISPLAY_OFF
void ST7567_FB_displayMode(uint8_t val)
{
  CS_ACTIVE;
  sendCmd(val);
  CS_IDLE;
}
// ----------------------------------------------------------------
void ST7567_FB_display(void)
{
  for(int y8=0; y8<SCR_HT8; y8++) {
    ST7567_FB_gotoXY(rotation?4:0,y8);
    //CS_ACTIVE;
    DC_DATA;
    for(int x=0; x<SCR_WD; x++) 
      sendSPI(scr[x+y8*SCR_WD]);
  }
  CS_IDLE;
}
// ----------------------------------------------------------------
// copy only part of framebuffer
void ST7567_FB_copy(uint8_t x, uint8_t y8, uint8_t wd, uint8_t ht8)
{
  uint8_t wdb = wd;
  for(int i=0; i<ht8; i++) {
    ST7567_FB_gotoXY(x+(rotation?4:0),y8+i);
    //CS_ACTIVE;
    DC_DATA;
    for(int x=0; x<wd; x++) sendSPI(*(scr+wdb*i+x));
  }
  CS_IDLE;
}
// ----------------------------------------------------------------
// ----------------------------------------------------------------
void ST7567_FB_cls(void)
{
  memset(scr,0,SCR_WD*SCR_HT8);
}
// ----------------------------------------------------------------
void ST7567_FB_drawPixel(uint8_t x, uint8_t y, uint8_t col) 
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  switch(col) {
    case 1: 
        scr[(y/8)*scrWd+x] |=   (1 << (y&7)); break;
    case 0: 
        scr[(y/8)*scrWd+x] &=  ~(1 << (y&7)); break;
    case 2: 
        scr[(y/8)*scrWd+x] ^=   (1 << (y&7)); break;
  }
}
// ----------------------------------------------------------------
void ST7567_FB_drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col)
{
  int dx = abs(x1-x0);
  int dy = abs(y1-y0);
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;
 
  while(1) {
    //if(x0>=0 && y0>=0) 
      ST7567_FB_drawPixel(x0, y0, col);
    if(x0==x1 && y0==y1) return;
    int err2 = err+err;
    if(err2>-dy) { err-=dy; x0+=sx; }
    if(err2< dx) { err+=dx; y0+=sy; }
  }
}
// ----------------------------------------------------------------
void ST7567_FB_drawLineH(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
  if(x1>x0) for(uint8_t x=x0; x<=x1; x++) 
                ST7567_FB_drawPixel(x,y,col);
  else      for(uint8_t x=x1; x<=x0; x++) 
                ST7567_FB_drawPixel(x,y,col);
}
// ----------------------------------------------------------------
void ST7567_FB_drawLineV(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
  if(y1>y0) for(uint8_t y=y0; y<=y1; y++) ST7567_FB_drawPixel(x,y,col);
  else      for(uint8_t y=y1; y<=y0; y++) ST7567_FB_drawPixel(x,y,col);
}
// ----------------------------------------------------------------
// about 4x faster than regular drawLineH
void ST7567_FB_drawLineHfast(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
  uint8_t mask;
  if(x1<x0) { mask=x0; x0=x1; x1=mask; } // swap
  mask = 1 << (y&7);
  switch(col) {
    case 1: for(int x=x0; x<=x1; x++) scr[(y/8)*scrWd+x] |= mask;   break;
    case 0: for(int x=x0; x<=x1; x++) scr[(y/8)*scrWd+x] &= ~mask;  break;
    case 2: for(int x=x0; x<=x1; x++) scr[(y/8)*scrWd+x] ^= mask;   break;
  }
}
// ----------------------------------------------------------------
// limited to pattern #8
void ST7567_FB_drawLineHfastD(uint8_t x0, uint8_t x1, uint8_t y, uint8_t col)
{
  uint8_t mask;
  if(x1<x0) { mask=x0; x0=x1; x1=mask; } // swap
  if(((x0&1)==1 && (y&1)==0) || ((x0&1)==0 && (y&1)==1)) x0++;
  mask = 1 << (y&7);
  switch(col) {
    case 1: for(int x=x0; x<=x1; x+=2) scr[(y/8)*scrWd+x] |= mask;   break;
    case 0: for(int x=x0; x<=x1; x+=2) scr[(y/8)*scrWd+x] &= ~mask;  break;
    case 2: for(int x=x0; x<=x1; x+=2) scr[(y/8)*scrWd+x] ^= mask;   break;
  }
}
// ----------------------------------------------------------------
uint8_t ST7567_FB_ystab[8]={0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80};
uint8_t ST7567_FB_yetab[8]={0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};
uint8_t ST7567_FB_pattern[4]={0xaa,0x55,0xaa,0x55};
// about 40x faster than regular drawLineV
void ST7567_FB_drawLineVfast(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
  //if(x < 0 || x > SCR_WD) return;
  if( x > SCR_WD) return;  
  int y8s,y8e;
  if(y1<y0) { y8s=y1; y1=y0; y0=y8s; } // swap
//  if(y0<0) y0=0;
  if(y0 > SCR_HT) y0=0;  
  if(y1>=SCR_WD) y1=SCR_WD-1;
  y8s=y0/8;
  y8e=y1/8;

  switch(col) {
    case 1: 
      if(y8s==y8e) scr[y8s*scrWd+x]|=(ystab[y0&7] & yetab[y1&7]);
      else { scr[y8s*scrWd+x]|=ystab[y0&7]; scr[y8e*scrWd+x]|=yetab[y1&7]; }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]=0xff;
      break;
    case 0:
      if(y8s==y8e) scr[y8s*scrWd+x]&=~(ystab[y0&7] & yetab[y1&7]);
      else { scr[y8s*scrWd+x]&=~ystab[y0&7]; scr[y8e*scrWd+x]&=~yetab[y1&7]; }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]=0x00;
      break;
    case 2: 
      if(y8s==y8e) scr[y8s*scrWd+x]^=(ystab[y0&7] & yetab[y1&7]);
      else { scr[y8s*scrWd+x]^=ystab[y0&7]; scr[y8e*scrWd+x]^=yetab[y1&7]; }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]^=0xff;
      break;
  }
}
// ----------------------------------------------------------------
// dithered version
void ST7567_FB_drawLineVfastD(uint8_t x, uint8_t y0, uint8_t y1, uint8_t col)
{
  int y8s,y8e;
  if(y1<y0) { y8s=y1; y1=y0; y0=y8s; } // swap
  y8s=y0/8;
  y8e=y1/8;

  switch(col) {
    case 1: 
      if(y8s==y8e) scr[y8s*scrWd+x]|=(ystab[y0&7] & yetab[y1&7] & pattern[x&3]);
      else { scr[y8s*scrWd+x]|=(ystab[y0&7] & pattern[x&3]); scr[y8e*scrWd+x]|=(yetab[y1&7] & pattern[x&3]); }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]|=pattern[x&3];
      break;
    case 0:
      if(y8s==y8e) scr[y8s*scrWd+x]&=~(ystab[y0&7] & yetab[y1&7] & pattern[x&3]);
      else { scr[y8s*scrWd+x]&=~(ystab[y0&7] & pattern[x&3]); scr[y8e*scrWd+x]&=~(yetab[y1&7] & pattern[x&3]); }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]&=~pattern[x&3];
      break;
    case 2: 
      if(y8s==y8e) scr[y8s*scrWd+x]^=(ystab[y0&7] & yetab[y1&7] & pattern[x&3]);
      else { scr[y8s*scrWd+x]^=(ystab[y0&7] & pattern[x&3]); scr[y8e*scrWd+x]^=(yetab[y1&7] & pattern[x&3]); }
      for(int y=y8s+1; y<y8e; y++) scr[y*scrWd+x]^=pattern[x&3];
      break;
  }
}
// ----------------------------------------------------------------
void ST7567_FB_drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  uint8_t drawVright=1;
  if(x+w>SCR_WD) { w=SCR_WD-x; drawVright=0; }
  if(y+h>SCR_HT) h=SCR_HT-y; else ST7567_FB_drawLineHfast(x, x+w-1, y+h-1,col);
  ST7567_FB_drawLineHfast(x, x+w-1, y,col);
  ST7567_FB_drawLineVfast(x,    y+1, y+h-2,col);
  if(drawVright) ST7567_FB_drawLineVfast(x+w-1,y+1, y+h-2,col);
}
// ----------------------------------------------------------------
// dithered version
void ST7567_FB_drawRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT) return;
  uint8_t drawVright=1;
  if(x+w>SCR_WD) { w=SCR_WD-x; drawVright=0; }
  if(y+h>SCR_HT) h=SCR_HT-y; else ST7567_FB_drawLineHfastD(x, x+w-1, y+h-1,col);
  ST7567_FB_drawLineHfastD(x, x+w-1, y,col);
  ST7567_FB_drawLineVfastD(x,    y+1, y+h-2,col);
  if(drawVright) ST7567_FB_drawLineVfastD(x+w-1,y+1, y+h-2,col);
}
// ----------------------------------------------------------------
void ST7567_FB_fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT || w<=0 || h<=0) 
      return;
  if(x+w>SCR_WD) 
      w=SCR_WD-x;
  if(y+h>SCR_HT) 
      h=SCR_HT-y;
  for(int i=y;i<y+h;i++) 
    ST7567_FB_drawLineH(x,x+w,i,col);
  
  //for(int i=x;i<x+w;i++) 
    //ST7567_FB_drawLineVfast(i,y,y+h-1,col);
}
// ----------------------------------------------------------------
// dithered version
void ST7567_FB_fillRectD(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t col)
{
  if(x>=SCR_WD || y>=SCR_HT || w<=0 || h<=0) return;
  if(x+w>=SCR_WD) w=SCR_WD-x;
  if(y+h>=SCR_HT) h=SCR_HT-y;
  for(int i=x;i<x+w;i++) ST7567_FB_drawLineVfastD(i,y,y+h-1,col);
}
// ----------------------------------------------------------------
// circle
void ST7567_FB_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t col)
{
  int f = 1 - (int)radius;
  int ddF_x = 1;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = radius;
 
  ST7567_FB_drawPixel(x0, y0 + radius, col);
  ST7567_FB_drawPixel(x0, y0 - radius, col);
  ST7567_FB_drawPixel(x0 + radius, y0, col);
  ST7567_FB_drawPixel(x0 - radius, y0, col);
 
  while(x < y) {
    if(f >= 0) {
      y--; ddF_y += 2; f += ddF_y;
    }
    x++; ddF_x += 2; f += ddF_x;    
    ST7567_FB_drawPixel(x0 + x, y0 + y, col);
    ST7567_FB_drawPixel(x0 - x, y0 + y, col);
    ST7567_FB_drawPixel(x0 + x, y0 - y, col);
    ST7567_FB_drawPixel(x0 - x, y0 - y, col);
    ST7567_FB_drawPixel(x0 + y, y0 + x, col);
    ST7567_FB_drawPixel(x0 - y, y0 + x, col);
    ST7567_FB_drawPixel(x0 + y, y0 - x, col);
    ST7567_FB_drawPixel(x0 - y, y0 - x, col);
  }
}
// ----------------------------------------------------------------
void ST7567_FB_fillCircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
  ST7567_FB_drawLineVfast(x0, y0-r, y0-r+2*r+1, col);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    ST7567_FB_drawLineVfast(x0+x, y0-y, y0-y+2*y+1, col);
    ST7567_FB_drawLineVfast(x0+y, y0-x, y0-x+2*x+1, col);
    ST7567_FB_drawLineVfast(x0-x, y0-y, y0-y+2*y+1, col);
    ST7567_FB_drawLineVfast(x0-y, y0-x, y0-x+2*x+1, col);
  }
}
// ----------------------------------------------------------------
// dithered version
void ST7567_FB_fillCircleD(uint8_t x0, uint8_t y0, uint8_t r, uint8_t col)
{
  ST7567_FB_drawLineVfastD(x0, y0-r, y0-r+2*r+1, col);
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    ST7567_FB_drawLineVfastD(x0+x, y0-y, y0-y+2*y+1, col);
    ST7567_FB_drawLineVfastD(x0+y, y0-x, y0-x+2*x+1, col);
    ST7567_FB_drawLineVfastD(x0-x, y0-y, y0-y+2*y+1, col);
    ST7567_FB_drawLineVfastD(x0-y, y0-x, y0-x+2*x+1, col);
  }
}
// ----------------------------------------------------------------
void ST7567_FB_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  ST7567_FB_drawLine(x0, y0, x1, y1, color);
  ST7567_FB_drawLine(x1, y1, x2, y2, color);
  ST7567_FB_drawLine(x2, y2, x0, y0, color);
}
#define swap(a, b) { int16_t t = a; a = b; b = t; }
// ----------------------------------------------------------------
// optimized for ST7567 native frame buffer
void ST7567_FB_fillTriangle( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, x, last;
  if (x0 > x1) { swap(y0, y1); swap(x0, x1); }
  if (x1 > x2) { swap(y2, y1); swap(x2, x1); }
  if (x0 > x1) { swap(y0, y1); swap(x0, x1); }

  if(x0 == x2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(y1 < a)      a = y1;
    else if(y1 > b) b = y1;
    if(y2 < a)      a = y2;
    else if(y2 > b) b = y2;
    ST7567_FB_drawLineVfast(x0, a, b, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  long sa = 0, sb = 0;

  if(x1 == x2) last = x1;
  else         last = x1-1;

  for(x=x0; x<=last; x++) {
    a   = y0 + sa / dx01;
    b   = y0 + sb / dx02;
    sa += dy01;
    sb += dy02;
    if(a > b) swap(a,b);
    ST7567_FB_drawLineVfast(x, a, b, color);
  }

  sa = dy12 * (x - x1);
  sb = dy02 * (x - x0);
  for(; x<=x2; x++) {
    a   = y1 + sa / dx12;
    b   = y0 + sb / dx02;
    sa += dy12;
    sb += dy02;
    if(a > b) swap(a,b);
    ST7567_FB_drawLineVfast(x, a, b, color);
  }
}
// ----------------------------------------------------------------
// optimized for ST7567 native frame buffer
void ST7567_FB_fillTriangleD( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, x, last;
  if (x0 > x1) { swap(y0, y1); swap(x0, x1); }
  if (x1 > x2) { swap(y2, y1); swap(x2, x1); }
  if (x0 > x1) { swap(y0, y1); swap(x0, x1); }

  if(x0 == x2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(y1 < a)      a = y1;
    else if(y1 > b) b = y1;
    if(y2 < a)      a = y2;
    else if(y2 > b) b = y2;
    ST7567_FB_drawLineVfastD(x0, a, b, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  long sa = 0, sb = 0;

  if(x1 == x2) last = x1;
  else         last = x1-1;

  for(x=x0; x<=last; x++) {
    a   = y0 + sa / dx01;
    b   = y0 + sb / dx02;
    sa += dy01;
    sb += dy02;
    if(a > b) swap(a,b);
    ST7567_FB_drawLineVfastD(x, a, b, color);
  }

  sa = dy12 * (x - x1);
  sb = dy02 * (x - x0);
  for(; x<=x2; x++) {
    a   = y1 + sa / dx12;
    b   = y0 + sb / dx02;
    sa += dy12;
    sb += dy02;
    if(a > b) swap(a,b);
    ST7567_FB_drawLineVfastD(x, a, b, color);
  }
}
// ----------------------------------------------------------------
const uint8_t ST7567_FB_ditherTab[4*17]  = {
  0x00,0x00,0x00,0x00, // 0

  0x00,0x00,0x00,0x88, // 1
  0x00,0x22,0x00,0x88, // 2
  0x00,0xaa,0x00,0x88, // 3
  0x00,0xaa,0x00,0xaa, // 4
  0x44,0xaa,0x00,0xaa, // 5
  0x44,0xaa,0x11,0xaa, // 6
  0x44,0xaa,0x55,0xaa, // 7
  
  0x55,0xaa,0x55,0xaa, // 8
  
  0xdd,0xaa,0x55,0xaa, // 9
  0xdd,0xaa,0x77,0xaa, // 10
  0xdd,0xaa,0xff,0xaa, // 11
  0xff,0xaa,0xff,0xaa, // 12
  0xff,0xee,0xff,0xaa, // 13
  0xff,0xee,0xff,0xbb, // 14
  0xff,0xee,0xff,0xff, // 15

  0xff,0xff,0xff,0xff  // 16
};

void ST7567_FB_setDither(int8_t s)
{
  if(s>16) s=16;
  if(s<-16) s=-16;
  if(s<0) {
    pattern[0] = ~*(ditherTab-s*4+0);
    pattern[1] = ~*(ditherTab-s*4+1);
    pattern[2] = ~*(ditherTab-s*4+2);
    pattern[3] = ~*(ditherTab-s*4+3);
  } else {
    pattern[0] = *(ditherTab+s*4+0);
    pattern[1] = *(ditherTab+s*4+1);
    pattern[2] = *(ditherTab+s*4+2);
    pattern[3] = *(ditherTab+s*4+3);
  }
}
// ----------------------------------------------------------------
#define ALIGNMENT \
  if(x==-1) x = SCR_WD-w; \
  else if(x<0) x = (SCR_WD-w)/2; \
  if(x<0) x=0; \
  if(x>=SCR_WD || y>=SCR_HT) return 0; \
  if(x+w>SCR_WD) w = SCR_WD-x; \
  if(y+h>SCR_HT) h = SCR_HT-y
// ----------------------------------------------------------------

int _ST7567_FB_drawBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h, Inverse_t invers)
{
  uint8_t wdb = w;
  ALIGNMENT;
  uint8_t i,y8,d,b,ht8=(h+7)/8;
  for(y8=0; y8<ht8; y8++) {
    for(i=0; i<w; i++) {
      d = *(bmp+wdb*y8+i);
      int lastbit = h - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
 //        if(d & 1) scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7));//исходный
          
//       if(invers == 0){
//           if(d & 1) 
//               scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7));
//           }
//       else{
//           if(d & 1) 
//               scr[((y+y8*8+b)/8)*scrWd+x+i] ^= (1 << ((y+y8*8+b)&7));     
//           }     

          
//if(invers == 2){
//ST7567_FB_drawPixel(x+i, y+y8,2); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
//}
//else{
//ST7567_FB_drawPixel(x+i, y+y8,  invers ); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
//}  


  switch(invers) {
    case 0: 
        if(d & 1)
            scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7)); break;
    case 1: 
        if(d & 0)
            scr[((y+y8*8+b)/8)*scrWd+x+i] &= ~(1 << ((y+y8*8+b)&7)); break;
    case 2: 
        if(d & 1)
            scr[((y+y8*8+b)/8)*scrWd+x+i] &= ~(1 << ((y+y8*8+b)&7)); 
        else 
            scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7));
        break;
  }

         d>>=1;
      }

    }
    
  }
  return x+w;
  


}

/*
Очистка окна с указаными  координатами
*/
int _ST7567_FB_clerBitmap(const uint8_t *bmp, int x, uint8_t y, uint8_t w, uint8_t h)
{
//  uint8_t wdb = w;
  ALIGNMENT;
  uint8_t i,y8,d,b,ht8=(h+7)/8;
  for(y8=0; y8<ht8; y8++) {
    for(i=0; i<w; i++) {
    //  d = *(bmp+wdb*y8+i);
       d =0; 
      int lastbit = h - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
         if(d & 1) 
             scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7));
         else    
             scr[((y+y8*8+b)/8)*scrWd+x+i] &= ~(1 << ((y+y8*8+b)&7));
         d>>=1;
      }
    }
  }
  return x+w;
  

}
//// ----------------------------------------------------------------
int ST7567_FB_drawBitmap(const uint8_t *bmp, int x, uint8_t y, Inverse_t invers)
{
  uint8_t w = *(bmp+0);
  uint8_t h = *(bmp+1);
  return _ST7567_FB_drawBitmap(bmp+2, x, y, w, h, invers);
}

/*
Гасим ранее выведенную картинку
*/
int ST7567_FB_clerBitmap(const uint8_t *bmp, int x, uint8_t y)
{
  uint8_t w = *(bmp+0);
  uint8_t h = *(bmp+1);
  return _ST7567_FB_clerBitmap(bmp+2, x, y, w, h);
}

/*
Инвертируем картинку
*/
int ST7567_FB_invertBitmap(const uint8_t *bmp, int x, uint8_t y)
{
  uint8_t w = *(bmp+0);
  uint8_t h = *(bmp+1);
    uint8_t wdb = w;
  ALIGNMENT;
  uint8_t i,y8,d,b,ht8=(h+7)/8;
  for(y8=0; y8<ht8; y8++) {
    for(i=0; i<w; i++) {
      d = *(bmp+wdb*y8+i);
      int lastbit = h - y8 * 8;
      if (lastbit > 8) lastbit = 8;
      for(b=0; b<lastbit; b++) {
        if(d & 1) 
             scr[((y+y8*8+b)/8)*scrWd+x+i] &= ~(1 << ((y+y8*8+b)&7));
         else    
             scr[((y+y8*8+b)/8)*scrWd+x+i] |= (1 << ((y+y8*8+b)&7));
         d>>=1;
      }
    }
  }
  return x+w;
}

// ----------------------------------------------------------------
// text rendering
// ----------------------------------------------------------------
void ST7567_FB_setFont(const FONT_INFO* font){
    cfont.CharacterHeight = font->CharacterHeight;
    cfont.StartCharacter = font->StartCharacter;
    cfont.EndCharacter = font->EndCharacter;
    cfont.spase = font->spase;
    cfont.Descriptors = font->Descriptors;
    cfont.Bitmaps = font->Bitmaps;
   
//{
//  cfont.font = font;
//  cfont.xSize = fontuint8_t(0);
//  cfont.ySize = fontuint8_t(1);
//  cfont.firstCh = fontuint8_t(2);
//  cfont.lastCh  = fontuint8_t(3);
//  cfont.minDigitWd = 0;
//  cfont.minCharWd = 0;
//  isNumberFun = &ST7567_FB_isNumber;
//  spacing = 1;
//  cr = 0;
//  invertCh = 0;
//    
//    
//}
    
}
    

/*
Печатаем одну букву
*/
//uint8_t ST7567_FB_printChar(char str){

//}
/*
Печатаем текст с переносом 
При отображении русского шрифта вместе с полезной информацией передаются непечатаемые символы(смотри грабли).
x,y - координаы точки экрана
*/
void ST7567_FB_printText(uint16_t x, uint16_t y, char *str, Inverse_t invers){
    uint16_t xpos = x;
    uint16_t ypos = y;
    uint16_t xSize = SCR_WD;//Узнакм ширину экрана
    uint16_t ySize = SCR_HT;//Узнакм ширину экрана
    FONT_CHAR_INFO *CharInfo;
    uint8_t CharIndex;
    uint8_t tmpStr = 0;
    while(*str){
    tmpStr = *str;
        if((uint8_t)*str == 0xD0 ){ 
            str++; 
            tmpStr = *str - 17;//Грабли для UTF-8, 17 это исключаем непечатаемые символы из таблицы
        }
        if((uint8_t)*str == 0xD1 ){ 
            str++; 
        tmpStr = *str - 17 + 64;} //64 это смещение по таблице для букв 'р' и дальше 
        
        CharIndex = tmpStr - cfont.StartCharacter;  //Порядковый номер буквы в массиве
        CharInfo = cfont.Descriptors;          //Point to start of descriptors
        CharInfo += (uint32_t)CharIndex/* * sizeof(FONT_CHAR_INFO)*/;   //Параметры текущей буквы
       
        //если буква не влезает в строку, то переходим на следующую строку
        if((xSize - xpos ) < (cfont.spase + CharInfo->CharWidth)){
            ypos += cfont.CharacterHeight;
            xpos = x;
        }  
        //если закончился экран, то переходим на начало экрана
        if((ySize - ypos) < cfont.CharacterHeight){
            ypos = 0;
            
        }
        
 //       uint8_t pos_byte = 0, pos_bit = 0;//координаты пикселя
        uint8_t dot_x = 0, dot_y = 0;//координаты пикселя
        dot_x = xpos;
        dot_y = ypos;
        int j=0;
        for(int i = 0; i < cfont.CharacterHeight ; i++){//+ (cfont.CharacterHeight * (CharInfo->CharWidth / 8))
            for( j = 0; j < CharInfo->CharWidth+0; j++){
                if(invers == 2){
                    ST7567_FB_drawPixel(dot_x+j, dot_y+i,2); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
                }
                else
                    ST7567_FB_drawPixel(dot_x+j, dot_y+i, cfont.Bitmaps[CharInfo->Offset + i*((CharInfo->CharWidth / 8) + ((CharInfo->CharWidth % 8) ? 1:0)) + (j/8)] & (1<<(7-(j%8))) ? 1^invers : 0^invers); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
            }
        }
        
        xpos += (CharInfo->CharWidth + 0);// сместились на ширину буквы  
        dot_x = xpos;
        dot_y = ypos;
        //Рисуем пробел между символами
         for(int i = 0; i < cfont.CharacterHeight ; i++){
         
            for( j = 0; j < cfont.spase; j++){
                if(invers == 2){
                    ST7567_FB_drawPixel(dot_x+j, dot_y+i,2); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
                }
                else
                    ST7567_FB_drawPixel(dot_x+j, dot_y+i, 1 & (1<<(7-(j%8))) ? 1^invers : 0^invers); //здесь i -  смещение по массиву, j - смещение по байту. Данные сдвинуты влевао поэто му 7-j
            }
         
         
         }
//        
        xpos += (cfont.spase);// сместились на ширину буквы  
        str++;  
    }
    
}






//--------------------------
  void ST7567_FB_setCR(uint8_t _cr) { cr = _cr; }
  void ST7567_FB_setInvert(uint8_t _inv) { invertCh = _inv; }
//  void ST7567_FB_setFontMinWd(uint8_t wd) { cfont.minCharWd = wd; }
//  void ST7567_FB_setCharMinWd(uint8_t wd) { cfont.minCharWd = wd; }
// ----------------------------------------------------------------
int ST7567_FB_fontHeight()
{
  return cfont.CharacterHeight;
}
// ----------------------------------------------------------------
int ST7567_FB_charWidth(FONT_CHAR_INFO *CharInfo, uint8_t *c)
{
    uint8_t CharIndex;

        CharIndex = *c - cfont.StartCharacter;  //Порядковый номер буквы в массиве
        CharInfo = cfont.Descriptors;          //Point to start of descriptors
        CharInfo += (uint32_t)CharIndex/* * sizeof(FONT_CHAR_INFO)*/;   //Параметры текущей буквы
       
   return CharInfo->CharWidth;
        
//  c = ST7567_FB_convertPolish(c);
//  if(c < cfont.firstCh || c > cfont.lastCh)
//    return c==' ' ?  1 + cfont.xSize/2 : 0;
//  if (cfont.xSize > 0) return cfont.xSize;
//  int ys8=(cfont.ySize+7)/8;
//  int idx = 4 + (c-cfont.firstCh)*(-cfont.xSize*ys8+1);
//  int wd = *(cfont.font + idx);
//  int wdL = 0, wdR = spacing; // default spacing before and behind char
//  if((*isNumberFun)(c) && cfont.minDigitWd>0) {
//    if(cfont.minDigitWd>wd) {
//      wdL = (cfont.minDigitWd-wd)/2;
//      wdR += (cfont.minDigitWd-wd-wdL);
//    }
//  } else if(cfont.minCharWd>wd) {
//    wdL = (cfont.minCharWd-wd)/2;
//    wdR += (cfont.minCharWd-wd-wdL);
//  }
//  return last ? wd+wdL+wdR : wd+wdL+wdR-spacing;  // last!=0 -> get rid of last empty columns 
}
// ----------------------------------------------------------------
int ST7567_FB_strWidth(char *str)
{
  int wd = 0;
    FONT_CHAR_INFO *CharInfo;
    uint8_t tmpStr = 0;
    while(*str){
        tmpStr = *str;
        if((uint8_t)*str == 0xD0 ){ 
            str++; 
            tmpStr = *str - 17;//Грабли для UTF-8, 17 это исключаем непечатаемые символы из таблицы
        }
        if((uint8_t)*str == 0xD1 ){ 
            str++; 
        tmpStr = *str - 17 + 64;} //64 это смещение по таблице для букв 'р' и дальше 
        
        
//        CharIndex = tmpStr - cfont.StartCharacter;  //Порядковый номер буквы в массиве
//        CharInfo = cfont.Descriptors;          //Point to start of descriptors
//        CharInfo += (uint32_t)CharIndex/* * sizeof(FONT_CHAR_INFO)*/;   //Параметры текущей буквы
        wd += ST7567_FB_charWidth(CharInfo, &tmpStr);
        wd += cfont.spase;
        str++; 
    }     

    return wd;
}
// ----------------------------------------------------------------
int ST7567_FB_printChar(int xpos, int ypos, unsigned char c)
{
    //scr[(y/8)*scrWd+x] |=   (1 << (y&7))
    
       
    
    
    
//  if(xpos >= SCR_WD || ypos >= SCR_HT)  return 0;
//  int fht8 = (cfont.ySize + 7) / 8, wd, fwd = cfont.xSize;
//  if(fwd < 0)  fwd = -fwd;

//  c = ST7567_FB_convertPolish(c);
//  if(c < cfont.firstCh || c > cfont.lastCh)  return c==' ' ?  1 + fwd/2 : 0;

//  int x,y8,b,cdata = (c - cfont.firstCh) * (fwd*fht8+1) + 4;
//  uint8_t d;
//  wd = fontuint8_t(cdata++);
//  int wdL = 0, wdR = spacing;
//  if((*isNumberFun)(c) && cfont.minDigitWd>0) {
//    if(cfont.minDigitWd>wd) {
//      wdL  = (cfont.minDigitWd-wd)/2;
//      wdR += (cfont.minDigitWd-wd-wdL);
//    }
//  } else if(cfont.minCharWd>wd) {
//    wdL  = (cfont.minCharWd-wd)/2;
//    wdR += (cfont.minCharWd-wd-wdL);
//  }

//  if(xpos+wd+wdL+wdR>SCR_WD) wdR = MAX(SCR_WD-xpos-wdL-wd, 0);
//  if(xpos+wd+wdL+wdR>SCR_WD) wd  = MAX(SCR_WD-xpos-wdL, 0);
//  if(xpos+wd+wdL+wdR>SCR_WD) wdL = MAX(SCR_WD-xpos, 0);

//  for(x=0; x<wd; x++) {
//    //uint8_t mask = 1 << ((xpos+x+wdL)&7);
//    for(y8=0; y8<fht8; y8++) {
//      d = fontuint8_t(cdata+x*fht8+y8);
//      int lastbit = cfont.ySize - y8 * 8;
//      if (lastbit > 8) lastbit = 8;
//      for(b=0; b<lastbit; b++) {
//         if(d & 1) scr[((ypos+y8*8+b)/8)*scrWd+xpos+x+wdL] |= 1<<((ypos+y8*8+b)&7);  //drawPixel(xpos+x+wdL, ypos+y8*8+b, 1);
//         d>>=1;
//      }
//    }
//  }
//  return wd+wdR+wdL;
}
// ----------------------------------------------------------------
int ST7567_FB_printStr(int xpos, int ypos, char *str)
{
//  unsigned char ch;
 // int stl, row;
  int x = xpos;
//  int y = ypos;
//  int wd = ST7567_FB_strWidth(str);

//  if(x==-1) // right = -1
//    x = SCR_WD - wd;
//  else if(x<0) // center = -2
//    x = (SCR_WD - wd) / 2;
//  if(x<0) x = 0; // left

//  while(*str) {
//    int wd = ST7567_FB_printChar(x,y,*str++);
//    x+=wd;
//    if(cr && x>=SCR_WD) { 
//      x=0; 
//      y+=cfont.ySize; 
//      if(y>SCR_HT) y = 0;
//    }
//  }
//  if(invertCh) ST7567_FB_fillRect(xpos,x-1,y,y+cfont.ySize+1,2);
  return x;
}
// ----------------------------------------------------------------
bool ST7567_FB_isNumber(uint8_t ch)
{
  return isdigit(ch) || ch==' ';
}
// ---------------------------------
bool ST7567_FB_isNumberExt(uint8_t ch)
{
  return isdigit(ch) || ch=='-' || ch=='+' || ch=='.' || ch==' ';
}

void ST7567_FB_setIsNumberFun(bool (*fun)(uint8_t)) { isNumberFun=fun; }

// ----------------------------------------------------------------
unsigned char ST7567_FB_convertPolish(unsigned char _c)
{
  unsigned char pl, c = _c;
  if(c==196 || c==197 || c==195) {
	  dualChar = c;
    return 0;
  }
  if(dualChar) { // UTF8 coding
    switch(_c) {
      case 133: pl = 1+9; break; // '�'
      case 135: pl = 2+9; break; // '�'
      case 153: pl = 3+9; break; // '�'
      case 130: pl = 4+9; break; // '�'
      case 132: pl = dualChar==197 ? 5+9 : 1; break; // '�' and '�'
      case 179: pl = 6+9; break; // '�'
      case 155: pl = 7+9; break; // '�'
      case 186: pl = 8+9; break; // '�'
      case 188: pl = 9+9; break; // '�'
      //case 132: pl = 1; break; // '�'
      case 134: pl = 2; break; // '�'
      case 152: pl = 3; break; // '�'
      case 129: pl = 4; break; // '�'
      case 131: pl = 5; break; // '�'
      case 147: pl = 6; break; // '�'
      case 154: pl = 7; break; // '�'
      case 185: pl = 8; break; // '�'
      case 187: pl = 9; break; // '�'
      default:  return c; 
    }
    dualChar = 0;
  } else   
  switch(_c) {  // Windows coding
    case 165: pl = 1; break; // �
    case 198: pl = 2; break; // �
    case 202: pl = 3; break; // �
    case 163: pl = 4; break; // �
    case 209: pl = 5; break; // �
    case 211: pl = 6; break; // �
    case 140: pl = 7; break; // �
    case 143: pl = 8; break; // �
    case 175: pl = 9; break; // �
    case 185: pl = 10; break; // �
    case 230: pl = 11; break; // �
    case 234: pl = 12; break; // �
    case 179: pl = 13; break; // �
    case 241: pl = 14; break; // �
    case 243: pl = 15; break; // �
    case 156: pl = 16; break; // �
    case 159: pl = 17; break; // �
    case 191: pl = 18; break; // �
    default:  return c; 
  }
  return pl+'~'+1;
}

/*
Возвращает координату X
0-слева
1-по центру
2-справа
*/
uint8_t ST7567_FB_align_text(char* buf, Align_t align){
    uint8_t len =   ST7567_FB_strWidth(buf);
    uint8_t pos_x = 0;
    switch(align){
      case ALIGN_LEFT:
          pos_x = 1;
          break;
      case ALIGN_CENTR:
            pos_x =  128 - len;
            pos_x /= 2;            
          break;      
      case ALIGN_RIGHT:
          pos_x =  128 - len;
          break;   
      default:
        break;          
    }

    return pos_x;
}
// ---------------------------------
 
void ST7567_FB_test(void){
for(int y8=0; y8<SCR_HT8; y8++) {
    ST7567_FB_gotoXY(rotation?4:0,y8);
    CS_ACTIVE;
    DC_DATA;
    for(int x=0; x<SCR_WD; x++) sendSPI(0xAA);
  }
  CS_IDLE;
  }
