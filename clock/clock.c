#include "clock.h"

/// @brief open a msg queue
/// @param name :the name of queue
/// @return -1:open fail / >=0:success, descriptor of queue
int open_msgQue(msg_queu_type queType)
{
	mqd_t mq = -1;
	struct mq_attr attr;

	memset(&attr, 0, sizeof(struct mq_attr));
	attr.mq_maxmsg = MAX_QUEMSG_NUM;
	attr.mq_msgsize = MAX_QUEUE_SIZE;

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
	assert(MAX_QUEUE_SIZE >= msg_len);
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
	assert(MAX_QUEUE_SIZE >= msg_len);

	retval = mq_receive(mq, msg_ptr, msg_len, 0);
	if(-1 == retval)
	{
		ERROR("recevie message failed");
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

int threadFun_check_time()
{
	return 0;
}
