#include "timer.h"

void uninit_time(int sec)  
{  
	struct itimerval value;  
	value.it_value.tv_sec = sec;  
	value.it_value.tv_usec = 0;  
	value.it_interval = value.it_value;  
	setitimer(ITIMER_REAL, &value, NULL);  
}

