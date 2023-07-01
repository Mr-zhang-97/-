#include "../clock/getTime.c"
struct st1{
	char a[10];
};
int main(void)
{
	int retval = 0;
	time_sec nowTime = 0;
	get_timeSec(&nowTime);
	INFO("now time is: %ld \n", nowTime);
	
	//test if printf(%s) can print a int or struct{int} -- no, cause they have different memory model;but can
	//print struct{char[] or char};
	//struct st1 s1 = {'a', 'b', '\0', 'c'};
	//INFO("now time is: %s \n", &s1);

	
	//test get_timeBrok;
	time_brok tb;
	memset(&tb, 0, sizeof(time_brok));
	retval = get_timeBrok(&nowTime, &tb);
	//tb = gmtime(&nowTime);
	if(0 != retval)
	{
		ERROR("get broken time failed .\n");
		return 0;
	}
	PRINT_BROKENTIME(tb);

	//get_timestr work
	char *time_str = NULL;
	time_str = get_timeStr(&nowTime);
	INFO("time string is: %s. \n", time_str);
	

	//test queue


	return retval;
}