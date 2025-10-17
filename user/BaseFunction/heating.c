#include "heating.h"
#include "pid.h"
#include "allDefenition.h"

/**/
void heatingStart(float turgetTemp){
	pid_init(&pid, 2.0, 0.1, 0.5, turgetTemp);
	deviceStatus.pidEnable = 1;
}

/**/
void heatingStop(void){
	deviceStatus.pidEnable = 0;
}
