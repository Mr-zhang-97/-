#include "clock.h"

//lock for all_schedule
pthread_mutex_t allSchedulLock = PTHREAD_MUTEX_INITIALIZER;
//lock for today_schedule
pthread_mutex_t todayScheduleLock = PTHREAD_MUTEX_INITIALIZER;
mqd_t gloable_msgQue_clock = 0;

/// @brief open a msg queue
/// @param name :the name of queue
/// @return -1:open fail / >=0:success, descriptor of queue
int open_msgQue(msg_queu_type queType)
{
	mqd_t mq = -1;
	struct mq_attr attr;

	memset(&attr, 0, sizeof(struct mq_attr));
	attr.mq_maxmsg = MAX_QUEMSG_NUM;
	attr.mq_msgsize = MAX_QUEUEMSG_LEN;

	switch (queType)
	{
	case clock_queue:
		mq = mq_open(MSG_QUEUE_CLOCK, O_CREAT | O_RDWR, 777, &attr);
		gloable_msgQue_clock = mq;
		break;
	default:
		break;
	}
	
	if((mqd_t)-1 == mq)
	{
		ERROR("%d, %d, %s", mq, errno,"open mq error \n");
		return -1;
	}
	return mq;
}

/// @brief unlink(remove) a msg queue
/// @param [in]queTyep : msg name 
/// @return 0 success / failed
int unlink_msgQue(msg_queu_type queTyep)
{
	int retval = 0;
	switch (queTyep)
	{
	case clock_queue:
		retval = mq_unlink(MSG_QUEUE_CLOCK);
		break;
	
	default:
		WARNING("queue type is uneffective .\n");
		break;
	}
	if(0 != retval)
	{
		//warning but not error for: unlink first then create;
		if(2 == errno)
		{
			WARNING("msg queue do not exist .\n");
			return 0;
		}
		else{
			ERROR("unlink msg queue failed errno is: %d.\n", errno);
			return -1;
		}
	}
	return 0;
}

/// @brief send msg to msg queue
/// @param [in]mq :msg queue descriptor 
/// @param [in]msg_ptr :message wait to send
/// @param [in]msg_len :length of message
/// @return -1 failed / success
int send_msgQue(mqd_t mq, const char* msg_ptr, size_t msg_len)
{
	assert(NULL != msg_ptr);
	assert(MAX_QUEUEMSG_LEN >= msg_len);
	assert(mq >=0);

	int retval = 0;

	if(msg_len != strlen(msg_ptr))
	{
		ERROR("msg len is: %lu, strlen(msg_ptr) is: %lu", msg_len, strlen(msg_ptr));
	}

	retval = mq_send(mq, msg_ptr, msg_len, 0);
	if(-1 == retval)
	{
		ERROR("send failed, error code is: %d \n", errno);
		return retval;
	}

	return retval;
}

/// @brief recevie message from msg queue
/// @param [in]mq :msg queue descriptor  
/// @param [in]msg_ptr :receive message buffer
/// @param [in]msg_len :length of message buffer
/// @return -1 failed / success, number of bytes of received message
int recv_msgQue(mqd_t mq, char *msg_ptr, size_t msg_len)
{
	int retval = 0;

	assert(msg_ptr);
	assert(MAX_QUEUEMSG_LEN >= msg_len);

	retval = mq_receive(mq, msg_ptr, msg_len, 0);
	if(-1 == retval)
	{
		ERROR("recevie message failed, errno:%d. err string:[%s]. \n", errno, strerror(errno));
		return retval;
	}

	return retval;
}

/// @brief create a thread in linux	//TODO check if function had been added to other thread, count it
/// @param [out]pid :pid of a thread
/// @param [in]start_fun :funtion run in thread
/// @param [in]arg :parameter for funtion 
/// @return 0 success / failed
int create_thread(pthread_t* pid, void*(*start_fun)(void*), void* arg)
{
	assert(start_fun);
	assert(pid);

	int retval = 0;
	retval = pthread_create(pid, NULL, start_fun, arg);
	if(0 != retval)
	{
		ERROR("create thread failed, errno is:%d.\n", retval);
		return retval;
	}
	return 0;
}


/// @brief join a thread to another one. //TODO check if this thread has been joined to another thread, this behaiver is undefined;
/// @param [in]pid : pid of thread waited to terminate 
/// @param [out]retval : exit status of terminated thread 
/// @return 0 success / failed
int join_thread(pthread_t pid, int* retval)
{
	assert(pid > 0);

	int ret = 0;
	ret = pthread_join(pid, (void**)&retval);
	if(0 != ret)
	{
		ERROR("join thread failed, errno:%d. \n", ret);
		return ret;
	}
	return 0;
}

/// @brief create a clock_list list head to recode clock_time; head node is not empty; count number from 1;
/// @param [in]pclockTime : time need to be recode into list;
/// @return NULL failed / pclock_list success;
clock_list* listNode_create(clock_time* pclockTime)
{
	assert(pclockTime);

	clock_list* pclock_list = malloc(sizeof(clock_list));
	if((0 != sizeof(clock_list) && pclock_list == NULL))
	{
		ERROR("malloc failed .\n");
		return NULL;
	}
	memset(pclock_list, 0, sizeof(clock_list));
	memcpy(&(pclock_list->clock_time), pclockTime, sizeof(clock_time));	

	pclock_list->size++;
	pclock_list->pnext = NULL;

	return pclock_list;
}

/// @brief free a whole clock_list from head;
/// @param [in]phead : head of a list; 
/// @return 0 success
int listNode_free(clock_list* phead)
{
	assert(phead);

	clock_list* ptmp_headNext = NULL;

	do
	{
		ptmp_headNext = phead->pnext;
		free(phead);
		phead = ptmp_headNext;
	} while (ptmp_headNext);

	return 0;
} 

/// @brief add a new node from head
/// @param [in]phead :list head 
/// @param [in]pclockTime :new node
/// @return :0 success
int listNode_add(clock_list* phead, clock_time* pclockTime)
{
	assert(pclockTime);
	assert(phead);

	clock_list* p_clock_list = NULL;
	p_clock_list = (clock_list*)malloc(sizeof(clock_list));
	memset(p_clock_list, 0, sizeof(clock_list));
	memcpy(&(p_clock_list->clock_time), pclockTime, sizeof(clock_time));
	p_clock_list->size++;

	p_clock_list->pnext = phead;
	phead = p_clock_list;

	return 0;
}

/// @brief insert item into list in order from small to big;
/// @param [in]phead 
/// @param [in]pclockTime 
/// @return 0 success
int listNode_add_inOrder(clock_list* phead, clock_time* pclockTime)
{
	assert(phead);
	assert(pclockTime);

	int retval = 0;
	int insertTime = 0;
	int nodeTime = 0;
	clock_list* ptmp_head = NULL;
	clock_list* ptmp_pre = NULL;
	clock_list* ptmp_next = NULL;

	insertTime = trans_timeBrok2timeSec((time_brok*)&pclockTime);

	//compare clock_time in second, insert item in order;
	ptmp_head = phead;
	while (ptmp_head)
	{
		ptmp_next = ptmp_head->pnext;
		nodeTime = trans_timeBrok2timeSec((time_brok*)&(ptmp_head->clock_time));
		//not insert here, move forward
		if(nodeTime <= insertTime)
		{
			ptmp_pre = ptmp_head;
			ptmp_head = ptmp_next;
		}
		else
		{
			INFO("insert here, phead size is: %d. \n", ptmp_head->size);
			break;
		}
	}

	//a new node;
	clock_list* ptmp_node = malloc(sizeof(clock_list));
	memset(ptmp_node, 0, sizeof(clock_list));
	memcpy(&(ptmp_node->clock_time), pclockTime, sizeof(clock_time));

	//insert in head;
	if(phead == ptmp_head){
		ptmp_node->pnext = phead;
		phead = ptmp_node;
		phead->size++;
		return 0;
	}

	//insert in end;
	if(NULL == ptmp_head){
		ptmp_pre->pnext = ptmp_node;
		phead->size++;
		return 0;
	}

	//insert in somewhere between middle;
	ptmp_pre->pnext = ptmp_node;
	ptmp_node->pnext = ptmp_head;
	phead->size++;

	return retval;
}

/// @brief delete the NO.n node from head, 1<= n <=size
/// @param [in]n :which one to be deleted from head; 
/// @param [in]phead :list 
/// @return 0 success 
clock_list* listNode_delete(int n, clock_list* phead)
{
	assert(phead);
	assert(n>=1);
	assert(n<=phead->size);

	clock_list* ptmp_head = NULL;
	clock_list* ptmp_pre = NULL;
	clock_list* ptmp_next = NULL;

	if(n == 1){
		ptmp_head = phead->pnext;
		free(phead);
		phead = ptmp_head;
		return phead;
	}
	ptmp_head = phead;
	ptmp_next = phead->pnext;
	while (n-1 != 0)
	{
		ptmp_head->size -- ;

		ptmp_pre = ptmp_head;
		ptmp_head = ptmp_next;
		ptmp_next = ptmp_head->pnext;

		n--;
	}
	ptmp_pre->pnext = ptmp_next;
	free(ptmp_head);

	return phead;
}

/// @brief y-m-d h-m-s, every item is empty, this clock_time is empty;
/// @param [in]p_clock_time 
/// @return 1 empty / 0 not
static int is_clockTimeEmpty(const clock_time* p_clock_time)
{
	if(0 == (p_clock_time->tm_year | p_clock_time->tm_mon | p_clock_time->tm_mday |
		p_clock_time->tm_hour | p_clock_time->tm_min | p_clock_time->tm_sec))
	{
		return 1;
	}
	return 0;
}

/// @brief delete all items in all_schedul;interface for users
/// @return 0 success
void clockTask_clear()
{
	//LOCK(allSchedulLock);
	memset(&all_schedul, 0, MAX_SCHEDULE_NUM*sizeof(clock_time));
	//UNLOCK(allSchedulLock);

}

/// @brief add an item to all_schedul, if its fulled, not insert but WARNING; interface for users
/// @param __clockTime 
/// @return 0 success;
int clockTask_add(clock_time __clockTime)
{
	int count = 0;
	char* sendStr = "hello";
	int retval = 0;

	sem_wait(&globle_clock_sem);
	for (count = 0; count < MAX_SCHEDULE_NUM; count++)
	{
		if (is_clockTimeEmpty(&(all_schedul[count])))
		{
			//LOCK(allSchedulLock);
			__clockTime.tm_year = __clockTime.tm_year - 1900;
			memcpy(&all_schedul[count], &__clockTime, sizeof(clock_time));
			//UNLOCK(allSchedulLock);
			break;
		}
	}
	if(count == MAX_SCHEDULE_NUM)
	{
		WARNING("all schedule is fulled, do not insert .\n");
	}

	//notify clock msg queue
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

/// @brief put task into today_schedule, task time should be in today`s range and bigger then now time;
/// @return 
static void* threadFun_check_task(void* arg)
{
	int retval = 0;
	char p_queMsg[MAX_QUEUEMSG_LEN] = {0};
	time_sec todayStartTime = 0;	//start time of a day: 0:0:0
	time_sec todayEndTime = 0;		//end time of a day: 23:59:59
	time_sec nowTime = 0;
	time_sec planTime = 0;

	while (1)
	{
		todayStartTime = get_startTime_today();
		if(-1 == todayStartTime)
		{
			ERROR("get_startTime_today failed .\n");
			retval = -1;
		}
		todayEndTime = get_endTime_today();
		if(-1 == todayEndTime)
		{
			ERROR("get_endTime_today failed .\n");
			retval = -1;
		}

		retval = recv_msgQue(gloable_msgQue_clock, p_queMsg, MAX_QUEUEMSG_LEN);
		if((mqd_t)retval == -1)
		{
			ERROR("recv_msgQue failed .\n");
		}else{
			get_timeSec(&nowTime);
			clock_time* ptmp_planTime = NULL;
			//find if any plan in all_schedul needed to be played today, put it into today_schedule;
			for(int i = 0; i < MAX_SCHEDULE_NUM; i++)
			{
				ptmp_planTime = &(all_schedul[i]);
				planTime = trans_timeBrok2timeSec((time_brok*)(ptmp_planTime));
				if(planTime >= nowTime && planTime <= todayEndTime)
				{
					listNode_add_inOrder(&today_schedule, ptmp_planTime);
					INFO("add task time to today schedule. \n");
				}else{
					WARNING("all_schedule[%d], time sec is:[%lu], not insert .\n", i, planTime);
				}
			}
		}

		get_timeSec(&nowTime);
		//update start time and end time of a day;
		if (nowTime > todayEndTime)
		{
			retval = get_startTime_today(todayStartTime);
			if (-1 == retval)
			{
				ERROR("get_startTime_today failed .\n");
				return NULL;
			}
			retval = get_endTime_today(todayEndTime);
			if (-1 == retval)
			{
				ERROR("get_endTime_today failed .\n");
				return NULL;
			}
		}
		sleep(1);
	}
	
	return NULL;
}

//we need clear goal to build clear task; date struct should be expandable
static void* threadFunc_exec_task(void* arg)
{
	time_sec nowTime = 0;
	time_sec planStartTime = 0;
	clock_list* ptmp_clock_list = NULL;

	while (1)
	{
		
		ptmp_clock_list = &today_schedule;
		while (ptmp_clock_list)
		{
			planStartTime = trans_timeBrok2timeSec((time_brok*)&(ptmp_clock_list->clock_time));
			get_timeSec(&nowTime);

			if(planStartTime >0 && planStartTime <= nowTime)
			{
				INFO("start to play clock audio, start play time:[%ld], now time:[%ld].\n", planStartTime, nowTime);
				sleep(10);
				INFO("play audio end, clock over .\n");
				//today_schedule is arranged in order, so execute and delete the first one;
				listNode_delete(1, &today_schedule);
				break;
			}
			ptmp_clock_list = ptmp_clock_list->pnext;
		}

		sleep(3);	
		
	}
	
	return NULL;
}

/// @brief start clock task;interface for users
/// @return -1 faild / 0 success
void* start_clock_task(void* arg)
{
	int retval = 0;
	pthread_t pid_checkTask = 0;
	pthread_t pid_execTask = 0;

	INIT_LOCK(allSchedulLock);
	INIT_LOCK(todayScheduleLock);

	//create clock msg queue 
	retval = unlink_msgQue(clock_queue);
	if(0 != retval)
	{
		ERROR("unlink msg queue failed .\n");
	}
	retval = open_msgQue(clock_queue);
	if(-1 == retval)
	{
		ERROR("open msg queue failed .\n");
	}
	gloable_msgQue_clock = (mqd_t)retval;

	memset(&all_schedul, 0, MAX_SCHEDULE_NUM*sizeof(clock_time));
	memset(&today_schedule, 0, sizeof(clock_list));
	sem_post(&globle_clock_sem);

	retval = create_thread(&pid_checkTask, &threadFun_check_task, NULL);
	if(0 != retval)
	{
		ERROR("create threadFun_check_task failed .\n");
	}
	
	retval = create_thread(&pid_execTask, &threadFunc_exec_task, NULL);
	if(0 != retval)
	{
		ERROR("create threadFunc_exec_task failed .\n");
	}
	INFO("create task end. \n");

	retval = join_thread(pid_checkTask, NULL);
	if(0 != retval)
	{
		ERROR("join threadFun_check_task failed .\n");
	}
	retval = join_thread(pid_execTask, NULL);
	if(0 != retval)
	{
		ERROR("join threadFunc_exec_task failed .\n");
	}

	return NULL;
}