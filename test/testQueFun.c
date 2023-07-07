#include "../clock/clock.c"
#include <semaphore.h>

pthread_mutex_t mutexxLock = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_a, sem_b;
int sum = 0;
int stop = 0;

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
//a funny question: print a and b in turn;
void printA(void)
{
	int j = 0;
	while(j < 25){
		sem_wait(&sem_a);
		j ++;
		printf("A \n");
		sem_post(&sem_b);
	}
}
void printB(void)
{
	int i = 0;
	while (i < 25)
	{
		sem_wait(&sem_b);
		i++;
		printf("B \n");
		sem_post(&sem_a);
	}
	
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
	/*
	retval = sendFunc();
	if(0 == retval)
	{
		retval = recvFunc();
		if(0 != retval)
		{
			ERROR("receive failed .\n");
		}
	}
	*/

	pthread_mutex_init(&mutexxLock, NULL);
	sem_init(&sem_a, 0, 1);
	sem_init(&sem_b, 0, 0);

	//test function of thread
	//print a and b in turns;
	pthread_t threadPid = 0;
	retval = create_thread(&threadPid, (void*)(&printA), NULL);
	if(0 != retval)
	{
		ERROR("create send thread failed .\n");
		return retval;
	}
	INFO("send pid is: %lu.\n", threadPid);
	
	pthread_t threadPid2 = 0;
	retval = create_thread(&threadPid2, (void*)(&printB), NULL);
	if(0 != retval)
	{
		ERROR("create recv thread failed .\n");
		return retval;	
	}
	INFO("recv pid is: %lu \n", threadPid2);

	retval = join_thread(threadPid, NULL);
	if(0 != retval)
	{
		ERROR("join send thread failed .\n");
		return retval;
	}
	retval = join_thread(threadPid2, NULL);
	if(0 != retval)
	{
		ERROR("join rece thread failed .\n");
		return retval;
	}
	INFO("create thread and send msg queue end .\n");
	
	
	return 0;
}
