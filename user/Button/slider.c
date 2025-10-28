#include "slider.h"
#include "allDefenition.h"
#include "hc595.h"

/**/
uint8_t checkSlider(uint16_t *sliderButton){
	static uint8_t power  = 0;
	static uint8_t led = 0;
if(*sliderButton == S1){
	led = 0;
	power  = 0;
}
//if(*sliderButton == S1+S2){
//led = 32;
//	power  = 1;
//}
if(*sliderButton == S2){
	led = 48;
	power  = 1;
}
if(*sliderButton == S3){
	led =56;
	power  = 2;
}
if(*sliderButton == S4){
	led = 60;
	power  = 3;
}
if(*sliderButton == S5){
	led = 62;
	power  = 4;
}
if(*sliderButton == S6){
	led = 63;
	power  = 5;
}
//led ^= 0xff;

	HC595_SendByte(&led);
return power;
}
