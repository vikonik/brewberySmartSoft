#include "slider.h"
#include "allDefenition.h"
#include "hc595.h"

/**/
void checkSlider(uint16_t *sliderButton){
	static uint8_t led = 0;
if(*sliderButton == S1){
	led = 0;
}
if(*sliderButton == S1+S2){
led = 32;
}
if(*sliderButton == S2){
	led = 48;
}
if(*sliderButton == S3){
	led =56;
}
if(*sliderButton == S4){
	led = 60;
}
if(*sliderButton == S5){
	led = 62;
}
if(*sliderButton == S6){
	led = 63;
}
//led ^= 0xff;

	HC595_SendByte(&led);
}
