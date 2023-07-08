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
int trans_timeSec2timeBrok(const time_sec* ptimeSec, time_brok* ptimeBrok)
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

char* trans_timeSec2timeStr(const time_sec* ptimeSec)
{
	assert(ptimeSec);

	return ctime(ptimeSec);
}

/// @brief translate time_broken to time second
/// @param [in]p_time_brok 
/// @return -1 failed / time_sec success;
time_sec trans_timeBrok2timeSec(time_brok* p_time_brok)
{
	assert(p_time_brok);

	return mktime(p_time_brok);
}

/// @brief get start time of today 0:0:0
/// @return -1 failed / success time_sec
long int get_startTime_today()
{
	time_sec tmp_timeSec = 0;
	time_brok tmp_timeBrok = {0};
	memset(&tmp_timeBrok, 0, sizeof(time_brok));
	int retval = 0;

	get_timeSec(&tmp_timeSec);
	retval = trans_timeSec2timeBrok(&tmp_timeSec, &tmp_timeBrok);
	if(-1 == retval)
	{
		ERROR("trans_timeSec2timeBrok failed .\n");
		return retval;
	}

	tmp_timeBrok.tm_hour = 0;
	tmp_timeBrok.tm_min = 0;
	tmp_timeBrok.tm_sec = 0;
	tmp_timeBrok.tm_isdst = 0;

	return trans_timeBrok2timeSec(&tmp_timeBrok);
}

long int get_endTime_today()
{
	time_sec tmp_timeSec = 0;
	time_brok tmp_timeBrok = {0};
	memset(&tmp_timeBrok, 0, sizeof(time_brok));
	int retval = 0;

	get_timeSec(&tmp_timeSec);
	retval = trans_timeSec2timeBrok(&tmp_timeSec, &tmp_timeBrok);
	if(-1 == retval)
	{
		ERROR("trans_timeSec2timeBrok failed .\n");
		return retval;
	}

	tmp_timeBrok.tm_hour = 23;
	tmp_timeBrok.tm_min = 59;
	tmp_timeBrok.tm_sec = 59;
	tmp_timeBrok.tm_isdst = 0;

	return trans_timeBrok2timeSec(&tmp_timeBrok);
}