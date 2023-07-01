#include "getTime.h"

typedef time_t time_sec;
typedef struct tm time_brok;

/// @brief show time in second
/// @param [out]ptime : -1 failed / success
/// @return 
void get_timeSec(time_sec* ptime)
{
	assert(ptime);

	time(ptime);
} 


/// @brief show time in a struct
/// @param [in]ptimeSec : time in second 
/// @param [out]ptimeBrok : time in struct
/// @return 0 success / -1 failed
int get_timeBrok(const time_sec* ptimeSec, time_brok* ptimeBrok)
{
	assert(ptimeSec);
	assert(ptimeBrok);

	time_brok* ptmp = NULL;

	//int retval = 0;
	//gmtime return a pointer point to a static allocalted struct which may be overwrite by another function
	//ptmp = gmtime(ptimeSec);
	ptmp = localtime(ptimeSec);
	if(NULL == ptmp)
	{
		return -1;
	}
	memcpy(ptimeBrok, ptmp, sizeof(time_brok));
	return 0;
}

char* get_timeStr(const time_sec* ptimeSec)
{
	assert(ptimeSec);

	return ctime(ptimeSec);
}