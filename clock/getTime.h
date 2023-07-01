#include <time.h>
#include <assert.h>
#include "logOutput.h"

typedef time_t time_sec;	//long int
typedef struct tm time_brok;

#define PRINT_BROKENTIME(__time_brok)	printf("broken time is: [%s], offset is:[%ld], timezone is:[%s] \n.", asctime(&__time_brok), __time_brok.tm_gmtoff, __time_brok.tm_zone) //input point of tm

void get_timeSec(time_sec *ptime);

int get_timeBrok(const time_sec *ptimeSec, time_brok *ptimeBrok);
