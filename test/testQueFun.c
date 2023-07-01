#include "../clock/clock.c"

static int sendFunc(void)
{
	char* sendStr = "hello";
	int retval = 0;
	if(gloable_msgQue_clock != 0)
	{
		INFO("gloable_msgQue_clock is: %d. \n", gloable_msgQue_clock);
		retval = send_msgQue(gloable_msgQue_clock, sendStr, strlen(sendStr));
		if(0 != retval)
		{
			ERROR("send queue failed.\n");
			return retval;
		}
	}
	return 0;
}
static int recvFunc(void)
{
	int retval = 0;
	char buf[10];
	memset(buf, 0, sizeof(buf));

	if(gloable_msgQue_clock != 0)
	{
		INFO("gloable_msgQue_clock is: %d. \n", gloable_msgQue_clock);
		retval = recv_msgQue(gloable_msgQue_clock, buf, sizeof(buf));
		if(-1 == retval)
		{
			ERROR("recv queue failed.\n");
			return retval;
		}
		else{
			INFO("buf is:%s, receive length is: %d. \n", buf, retval);
		}
	}
	return 0;
}
int main()
{
	int retval =  0;
	retval = unlink_msgQue(clock_queue);
	if(0 != retval)
	{
		ERROR("unlink msg queue failed .\n");
		return -1;
	}
	retval = open_msgQue(clock_queue);
	if(-1 == retval)
	{
		ERROR("open msg queue failed .\n");
		return 0;
	}
	gloable_msgQue_clock = (mqd_t)retval;
	INFO("gloable_msgQue_clock is: %d. \n", (mqd_t)retval);

	retval = sendFunc();
	if(0 == retval)
	{
		retval = recvFunc();
		if(0 != retval)
		{
			ERROR("receive failed .\n");
		}
	}
	
	return 0;
}