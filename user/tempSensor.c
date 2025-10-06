#include "tempSensor.h"
#include "ds18b20.h"

 

float TemperatureCorrection;
	float TemperatureCorrectionMin;
	float TemperatureCorrectionMax;
/**/
void tempSensorInit(void)
{
	TemperatureCorrection = 0;
	TemperatureCorrectionMin = -5;				
	TemperatureCorrectionMax = 5;
	
	DS18B20_Init(&ds18b20);
	
	if(DS18B20_InitializationCommand(&ds18b20) != DS18B20_ERROR){
		ds18b20.isSensorError = 0;
		DS18B20_ReadRom(&ds18b20);
		DS18B20_ReadScratchpad(&ds18b20);

		uint8_t settings[3];
		settings[0] = ds18b20.temperatureLimitHigh;
		settings[1] = ds18b20.temperatureLimitLow;
		settings[2] = DS18B20_12_BITS_CONFIG;

		DS18B20_InitializationCommand(&ds18b20);
		DS18B20_SkipRom(&ds18b20);
		DS18B20_WriteScratchpad(&ds18b20, settings);
	}
	else{
		ds18b20.isSensorError = 1;
	}
}

/**/
void readTemperature(void)
{
	
		if(DS18B20_InitializationCommand(&ds18b20) != DS18B20_ERROR){
			ds18b20.isSensorError = 0;
			DS18B20_SkipRom(&ds18b20);
			DS18B20_ReadScratchpad(&ds18b20);							
			ds18b20.temperature += TemperatureCorrection;
			DS18B20_InitializationCommand(&ds18b20);
			DS18B20_SkipRom(&ds18b20);
			DS18B20_ConvertT(&ds18b20, DS18B20_NONE);			
		}
		else{
			ds18b20.isSensorError = 1;
		}
}

